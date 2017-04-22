CFLAGS = -g -Wall
CC = gcc

all: test node

test: lsPacket.c lsGraph.c lsDijkstra.c lsNetwork.c
	$(CC) $(CFLAGS) lsPacket.c lsGraph.c lsDijkstra.c lsNetwork.c test.c -o test

node: lsPacket.c lsGraph.c lsDijkstra.c lsNetwork.c
	$(CC) $(CFLAGS) -pthread lsPacket.c lsGraph.c lsDijkstra.c lsNetwork.c node.c -o node

.PHONY: clean
clean:
	rm test node