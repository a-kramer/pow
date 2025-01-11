# Powers and Hats

This program substitutes `a^b` to `pow(a,b)`.

There is an unofficial (almost-) consensus of human-plain-text-math.
Most humans would write `2^3` in emails and other text documents
(without proof), using the caret (`U+005E`) operator (or _hat_):
`a^b`.

This is difficult to do with regular expressions
(e.g. `\(?([[:alnum:]]+)\?\^([[:alnum:]]+)`, capturing base and
exponent). This is especially difficult because of nested
sub-expressions:

$$ \left(1+\left(1+(1+a)\right)\right)^3\,, $$

where a regex would struggle to find the correct parenthesis to capture the sub-group. As text, this is:

```
(1+(1+(1+a)))^3
```

So, a small program that counts opening and closing parentheses will be easier than many regular expressions for the various difficult cases of base and exponent.

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
while `^` is interpreted as _xor_ (exclusive or).

Some languages lack a fundamental power operation entirely (GNU Guile, Lisp):

```elisp
;; does not work:
; (^ 2 3)
; (** 2 3)

(exp (* 3 (log 2)))
;7.999999999999998
```

In C, powers are written either as `pow(a, b)`, or  with more specific integer power functions from the GNU Scientific Library:

```c
pow(double a, double b);
gsl_pow_[2-9](double a);
gsl_pow_int(double a, int b);
exp(b*log(a)); /* also ok */
```

So, this program makes this substitution from _human-plain-text_ math to c-math (with GSL functions if appropriate, maybe not perfectly).
