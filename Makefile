CFLAGS = -g -Wall
CC = gcc

all: test

test: lsPacket.c lsGraph.c lsDijkstra.c
	$(CC) $(CFLAGS) lsPacket.c lsGraph.c lsDijkstra.c test.c -o test

.PHONY: clean
clean:
	rm test