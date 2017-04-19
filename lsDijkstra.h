/**
 * This file describes the functions used for finding the shortest path via Dijkstra's Algorithm
 *
 * @author Jeffrey Bromen
 * @date 4/17/17
 * @info Systems and Networks II
 * @info Project 3
 */

#ifndef _LSDIJKSTRA_H
#define _LSDIJKSTRA_H

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "lsGraph.h"

#define PARENT(i) (int) ((i - 1) / 2)
#define LCHILD(i) i * 2 + 1
#define RCHILD(i) i * 2 + 2 

struct heapNode {
	int value;
	struct tableEntry *entry;
} heapNode;

struct priorityQueue {
	int size;
	int max;
	struct heapNode *heap;
} priorityQueue;

struct routingTable {
	int size;
	int numEntries;
	struct tableEntry **table;
} routingTable;

struct tableEntry {
	struct vertexT *vertex;
	struct vertexT *parent;
	int cost;
} tableEntry;

void heapify(struct heapNode *heap, int count);

void siftDown(struct heapNode *heap, int start, int end);

void siftUp(struct heapNode *heap, int start);

void swap(struct heapNode *h1, struct heapNode *h2);

struct priorityQueue *newPriorityQueue(int max);

void enqueue(struct priorityQueue *pq, struct tableEntry *entry);

struct tableEntry *dequeue(struct priorityQueue *pq);

struct routingTable *newRoutingTable(int size);

struct tableEntry *newTableEntry(struct vertexT *vertex);

int tableContains(struct routingTable *table, char destination);

void addEntryToTable(struct routingTable *table, struct tableEntry *entry);

void dijkstra(struct routingTable *table, struct graphT *graph, char start);

void printRoutingTable(struct routingTable *table);

#endif // _LSDIJKSTRA_H