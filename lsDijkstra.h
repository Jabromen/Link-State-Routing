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

struct MinHeap
{
	int size;
	int cap;
	int *pos;
	struct HeapNode **array;
} MinHeap;

struct HeapNode
{
	int index;
	int cost;
} HeapNode;

/**
 * Initialize a new MinHeap structure.
 *
 * @param cap - capacity of min heap
 *
 * @return - pointer to MinHeap structure
 */
struct MinHeap *newMinHeap(int cap);

/**
 * Initializes a new HeapNode structure.
 *
 * @param index - index of graph node
 * @param cost  - cost to reach node
 *
 * @return - pointer to HeapNode structure
 */
struct HeapNode *newHeapNode(int index, int cost);

/**
 * Fixes a min heap by sifting up from index
 *
 * @param heap  - min heap being fixed
 * @param index - starting index
 */
void siftUp(struct MinHeap *heap, int index);

/**
 * Fixes a min heap by sifting down from index
 *
 * @param heap  - min heap being fixed
 * @param index - starting index
 */
void siftdown(struct MinHeap *heap, int index);

/**
 * Swaps two HeapNode pointers. Helper function for heap operations.
 *
 * @param a - node #1 being swapped
 * @param b - node #2 being swapped
 */
void swap(struct HeapNode **a, struct HeapNode **b);

/**
 * Inserts a HeapNode into the correct place in a MinHeap
 *
 * @param heap - min heap being modified
 * @param node - node being inserted
 */
void insert(struct MinHeap *heap, struct HeapNode *node);

/**
 * Removes the node with the smallest cost from a MinHeap.
 *
 * @param heap - heap from which node is extracted
 *
 * @return - pointer to extracted node
 */
struct HeapNode *extract(struct MinHeap *heap);

/**
 * Updates the cost of a node in the heap and repairs the heap.
 *
 * @param heap  - heap being updated
 * @param index - index of node
 * @param cost  - new cost of node
 */
void updateHeap(struct MinHeap *heap, int index, int cost);

/**
 * Checks if a node is currently in the heap.
 *
 * @param heap  - heap being checked
 * @param index - index of node being searched for
 *
 * @return - 1 if in heap, 0 if not in heap
 */
int isInHeap(struct MinHeap *heap, int index);

/**
 * Checks to see if a heap is empty.
 *
 * @param heap - heap being checked
 *
 * @return - 1 if empty, 0 if not empty
 */
int isEmpty(struct MinHeap *heap);

/**
 * Computes the shortest path across a graph.
 *
 * @param graph  - graph being analyzed
 * @param source - label of starting node
 */
void dijkstra(struct Graph *graph, char source);

#endif // _LSDIJKSTRA_H