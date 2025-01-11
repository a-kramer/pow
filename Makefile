CC=cc
CFLAGS=-O2 -march=native -Wall

.PHONY: all


all: rp


rp: replace_power.c
	$(CC) $(CFLAGS) -o $@ $^
