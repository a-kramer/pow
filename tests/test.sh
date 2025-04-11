#!/bin/sh

RP="./rp"
FMT="%25s "
[ -f "$RP" ] || exit 1;

# 1: math expression
# 2: expected result
Compare () {
	ORIGINAL=`echo "$1" | tr -d ' '`
	EXPECTED=`echo "$2" | tr -d ' '`
	OUT=`$RP "$ORIGINAL" | tr -d ' '`
	printf "$FMT" "$ORIGINAL"
	printf "$FMT" "$EXPECTED"
	printf "$FMT" "$OUT"
 [ "$OUT" = "$EXPECTED" ] &&  echo ' \e[32msuccess\e[0m' || echo ' \e[31mfailure\e[0m'
}

printf "$FMT$FMT$FMT\n" "ORIGINAL" "EXPECTED" "ACTUAL"

Compare "a^b" "pow(a, b)"
Compare "a^2" "gsl_pow_2(a)"
Compare "(1+(1+a))^b" "pow(1+(1+a), b)"
Compare "a^((2))" "pow(a, (2))"
Compare "a+(b+((c))^g)" "a+(b+pow((c), g))"
Compare "a+(b+(c)^g)" "a+(b+pow(c, g))"
Compare "a+var^20" "a+gsl_pow_int(var, 20)"
Compare "a+b^(1+c)+d" "a+pow(b, 1+c)+d"
Compare "a^1.3e5" "pow(a, 1.3e5)"
Compare "(a^1.3e-5)" "(pow(a, 1.3e-5))"
Compare "(a^(1.3e-5))" "(pow(a, 1.3e-5))"
