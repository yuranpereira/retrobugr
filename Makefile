CC=gcc
CFLAGS=-g -O0 -Ideps/linenoise
#CFLAGS=-std=c11 -g -Ideps/linenoise

all: retrobugr

retrobugr: src/retrobugr.c deps/linenoise/linenoise.c
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -f retrobugr
