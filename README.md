This program substitutes `a^b` to `pow(a,b)`.

```sh
make
```

With `pow`:

```sh
$ ./rp a^b
pow(a,b)
$ ./rp '(1+(1+(1+a)))^(b/c)'
pow(1+(1+(1+a)), b/c)
```

And alternatively with `exp` and `log`:

```sh
$ ./rp -e a^b
exp(b*log(a))
$ ./rp -e 'a^(b+c)'
exp((b+c)*log(a))
```

# Powers and Hats

There is an unofficial (almost-) consensus of human-plain-text-math.
Most humans would write `2^3` in emails and other text documents
(without proof), using the caret (`U+005E`) operator (or _hat_):
`a^b`. LaTeX uses `^`, so it can't be wrong. In some cases known
integer powers can be written as `a²`, but this is not usually done in
plain text. Maybe there should be a more strict consensus around this.

The `a**b` is probably even a little better in some sense. On one hand
it is a two letter operator, thus breaking the pattern of one letter
opertors `+` `-` `*` `/` and then `**`. On the other hand, integer
powers are repeated multiplications: `a*a*a == a**3`, so it looks self
explanatory. And of course `^` can mean xor as well, which is never
the case for `**`. However, `++` is not integer multiplication: `a+a+a
== a++3`, so this also breaks patterns and would imply that `a++++2`
would also be _a squared_. :/

There are other conventions as well and sometimes it is necessary
to parse powers and convert them to functions.

## Regular Expressions

Matching powers is difficult to do with regular expressions, e.g.:
`\(?([[:alnum:]]+)\)?\^\(?([[:alnum:]]+)\)?`, with capture groups for _base_ and
_exponent_.

The above expression will match these (some of which erroneosly):

```sh
a^b
(a^b)
a)^b
(a)^b)
```

These all match regardless of unmatched parnetheses. There is no
regex for _please only consider matched parentheses_. And the capture
groups also need to catch operators, of course: `(1+a)^(1/3)`.

The only workaround I can think of is to use multiple, correctly
ordered expressions, with mandatory matched parentheses, or no
parentheses, similar to (but more than that):

```sed
s/([[:alnum:]]+)^([[:alnum:]]+)/pow(\1,\2)/g
s/\(([^()]+)\)^([[:alnum:]]+)/pow(\1,\2)/g
s/\(([^()]+)\)^\(([^()]+)\)/pow(\1,\2)/g
s/([[:alnum:]])^\(([^()]+)\)/pow(\1,\2)/g
```
And this only covers one set of parentheses, no nesting.
Maybe there are tricks to avoid repeating very similar patterns, but I don't know them.

Nested expressions make this task especially difficult:

$$ \left(1+\left(1+(1+a)\right)\right)^3\,, $$

where a regex would struggle to find the correct parenthesis to capture the base sub-group. As text, this is:

```
(1+(1+(1+a)))^3
```
### Conclusion

A small program that counts opening and closing parentheses will be
much easier to write. But, we have to be careful about copying
strings, allocating buffers and preserving or omitting the parnetheses
appropriately

## Languages and Powers

In many programming languages, powers are written like that as well (R, GNU Octave, awk, dc, bc):

```sh
dc -e '2 3 ^ p'
# 8
echo "2^3" | bc
# 8
awk 'BEGIN {print 2^3}'
# 8
```

In some languages, powers are written as `a**b` (fortran, gnuplot, python, perl):

```sh
perl -e 'printf("2^3=%i\n", 2^3); printf("2**3=%i\n", 2**3);'
# 2^3=1
# 2**3=8
```
while `^` is interpreted as bitwise exclusive OR.

Some languages lack a fundamental power operation entirely (GNU Guile, Lisp):

```elisp
;; does not work:
; (^ 2 3)
; (** 2 3)

(exp (* 3 (log 2)))
;7.999999999999998
```

Substituting `^` for `**` is trivial. Same goes for the `exp` and `log` functions, they look the same in almost any programming language. In some, they are capitalized, or use the ln spelling for natual logarithms: `Exp()`, `Ln()` in yacas. These substitutions are also trivial.

## C

In C, powers are written either as `pow(a, b)`, or  with more specific integer power functions from the GNU Scientific Library:

```c
pow(double a, double b);
gsl_pow_[2-9](double a);
gsl_pow_int(double a, int b);
exp(b*log(a)); /* also ok */
```

So, this program makes this substitution from _human-plain-text_ math to c-math (with GSL functions if appropriate, maybe not perfectly);

```sh
./rp '1+(1+(1+a))^3+a^b'
# 1+gsl_pow_3(1+(1+a))+pow(a, b)
```

Too many parentheses in the exponent shortcut to `pow`, even if the parentheses embrace an integer:

```sh
./rp '1+(1+(1+a))^3 + a^20 + (1+a)^((3)) + a^b'
1+gsl_pow_3(1+(1+a)) + gsl_pow_int(a, 20) + pow(1+a, (3)) + pow(a, b)
```

# Undecided case that is difficult to implement correctly:

Unary minuses are difficult to parse by just counting parentheses:

```sh
$ ./rp '-a^b ; a-b^3 ; -1.0^4 ; a^-1'
-pow(a, b) ; a-gsl_pow_3(b) ; -gsl_pow_4(1.0) ; pow(a, -1)
```

Some of these may appear wrong to you: is it $(-1)^4$ or $-(1^4)$?
It's the latter. You can think of it like this: `^` has the highest
precedence, higher than unary minus.

If you mean something different, you need to make it explicit:

```sh
$ ./rp '(-a)^b ; a-b^3 ; (-1.0)^4 ; a^-1'
pow(-a, b) ; a-gsl_pow_3(b) ; gsl_pow_4(-1.0) ; pow(a, -1)
```

## Corollary

The `dc` program has no precedence to its operators at all, and _still_ unary minuses are a problem:

```sh
$ echo '1 -2 + p' | dc
dc: stack empty
3
```

because `-` isn't the unary minus, `_` is:

```sh
$ echo '1 _2 + p' | dc
-1
```

... but only in the inut, with hilarious consequences. Say
$$f(a,b) = (a+b) - 1*(a+b < 0),$$
and we want to call `dc` twice, to cover the
inequality cases (otherwise we could do the entire function in one dc call of course).

```sh
$ f () {
  x=`echo "$1 $2 + p" | dc`
  y=`[ $x -lt 0 ] && echo "$x 1 - p" | dc || echo "$x"`
  echo "$1 $2 -> $y"
}

$ f 1 2
1 2 -> 3

$ f 1 _2
dc: stack empty
1 _2 -> 0

$ f 1 -2
dc: stack empty
1 -2 -> 3
```

So, one has to go back and forth in notation:

```bash
$ g () {
  x=`echo "$1 $2 + p" | dc`
  y=`[ $x -lt 0 ] && echo "${x//-/_} 1 - p" | dc || echo "$x"`
  echo "$1 $2 -> $y"
}

$ f 1 _3
dc: stack empty
1 _3 -> 1
$ g 1 _3
1 _3 -> -3
```

where `${x//-/_}` is a bash feature and doesn't work in a strict POSIX
compliant shell (then `tr - _`).

### Note

Issues around unary minuses are not so rare as to be exceptions. I
have seen scientific models that didn't work right, because they were
auto-exported from one language/format into another, with unary
minuses missing.

# Build

A Makefile is included:

```sh
make && make test
```

