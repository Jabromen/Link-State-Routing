CFLAGS = -g -Wall
CC = gcc

all: test

test: lsPacket.c
	$(CC) $(CFLAGS) lsPacket.c test.c -o test

.PHONY: clean
clean:
	rm test