CFLAGS = -g -Wall
CC = gcc

all: test

test: lsPacket.c lsGraph.c
	$(CC) $(CFLAGS) lsPacket.c lsGraph.c test.c -o test

.PHONY: clean
clean:
	rm test