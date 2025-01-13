CC=gcc
CFLAGS=-O2 -march=native -Wall

.PHONY: all test


all: rp


rp: replace_power.c
	$(CC) $(CFLAGS) -o $@ $^

test: rp
	tests/test.sh
