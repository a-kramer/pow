# Powers and Hats

This program substitutes `a^b` to `pow(a,b)`.

There is an unofficial (almost-) consensus of human-plain-text-math.
Most humans would write `2^3` in emails and other text documents
(without proof), using the caret (`U+005E`) operator (or _hat_):
`a^b`.

But, there are other conventions and sometimes it is necessary to
parse powers and convert them to functions.

This is difficult to do with regular expressions, e.g.:
`\(?([[:alnum:]]+)\)?\^\(?([[:alnum:]]+)\)?`, to capture base and
exponent.

The above expression will match these (some of which erroneosly):

```sh
a^b
(a^b)
a)^b
(a)^b)
```

These all match despite having unmatched parnetheses. There is no regex for _please only consider matched parentheses_. And the capture groups also need to catch operators, of course: `(1+a)^(1/3)`.

The only workaround is to use multiple, correctly ordered expressions, with mandatory matched parentheses, or no parentheses, similar to this (but more of these):

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

A small program that counts opening and closing parentheses will be much easier to write.

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

# Build

A Makefile is included:

```sh
make && make test
```
