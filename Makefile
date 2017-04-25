CFLAGS = -g -Wall
CC = gcc

all: node

node: lsPacket.c lsGraph.c lsDijkstra.c lsNetwork.c
	$(CC) $(CFLAGS) -pthread lsPacket.c lsGraph.c lsDijkstra.c lsNetwork.c node.c -o node

.PHONY: clean
clean:
	rm node