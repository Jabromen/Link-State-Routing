/**
 * This file implements the functions used for finding the shortest path via Dijkstra's Algorithm
 *
 * @author Jeffrey Bromen
 * @date 4/17/17
 * @info Systems and Networks II
 * @info Project 3
 */

#include "lsDijkstra.h"

struct MinHeap *newMinHeap(int cap)
{
	struct MinHeap *heap = (struct MinHeap *) malloc(sizeof(struct MinHeap));

	if (!heap)
		return NULL;

	if (!(heap->pos = (int *) malloc(cap * sizeof(int))))
	{
		free(heap);
		return NULL;
	}

	if (!(heap->array = (struct HeapNode **) malloc(cap * sizeof(struct HeapNode *))))
	{
		free(heap->pos);
		free(heap);
		return NULL;
	}

	heap->size = 0;
	heap->cap = cap;

	return heap;
}

struct HeapNode *newHeapNode(int index, int cost)
{
	struct HeapNode *node = (struct HeapNode *) malloc(sizeof(struct HeapNode));

	if (!node)
		return NULL;

	node->index = index;
	node->cost = cost;

	return node;
}

void siftUp(struct MinHeap *heap, int index)
{
	if (index > heap->size)
		return;

	int ind, parent, temp;
	struct HeapNode *indNode, *parentNode;

	ind = index;
	while (ind > 0)
	{
		parent = PARENT(ind);
		if (heap->array[ind]->cost < heap->array[parent]->cost)
		{
			indNode = heap->array[ind];
			parentNode = heap->array[parent];

			temp = heap->pos[indNode->index];
			heap->pos[indNode->index] = heap->pos[parentNode->index];
			heap->pos[parentNode->index] = temp;

			swap(&heap->array[ind], &heap->array[parent]);

			ind = parent;
		}
		else
			break;
	}
}

void siftDown(struct MinHeap *heap, int index)
{
	int small, lChild, rChild, temp;
	struct HeapNode *smallNode, *indexNode;

	small = index;
	lChild = LCHILD(small);
	rChild = RCHILD(small);

	if (lChild < heap->size && heap->array[lChild]->cost < heap->array[small]->cost)
		small = lChild;

	if (rChild < heap->size && heap->array[rChild]->cost < heap->array[small]->cost)
		small = rChild;

	if (small != index)
	{
		indexNode = heap->array[index];
		smallNode = heap->array[small];

		temp = heap->pos[indexNode->index];
		heap->pos[indexNode->index] = heap->pos[smallNode->index];
		heap->pos[smallNode->index] = temp;

		swap(&heap->array[index], &heap->array[small]);

		siftDown(heap, small);
	}
}

void swap(struct HeapNode **a, struct HeapNode **b)
{
	struct HeapNode *temp = *a;

	*a = *b;
	*b = temp;
}

void insert(struct MinHeap *heap, struct HeapNode *node)
{
	if (heap->size >= heap->cap)
		return;

	heap->array[heap->size] = node;
	heap->pos[node->index] = heap->size;
	heap->size++;

	siftUp(heap, heap->size - 1);
}

struct HeapNode *extract(struct MinHeap *heap)
{
	if (isEmpty(heap))
		return NULL;

	struct HeapNode *node = heap->array[0];

	heap->array[0] = heap->array[heap->size - 1];

	heap->pos[node->index] = heap->size - 1;
	heap->pos[heap->array[0]->index] = 0;

	heap->size--;
	siftDown(heap, 0);

	return node;
}

void updateHeap(struct MinHeap *heap, int index, int cost)
{
	int i = heap->pos[index];
	int op = heap->array[i]->cost > cost;

	heap->array[i]->cost = cost;

	// If lowering cost, sift up
	if (op)
		siftUp(heap, i);
	// If raising cost, sift down
	else
		siftDown(heap, i);
}

int isInHeap(struct MinHeap *heap, int index)
{
	return heap->pos[index] < heap->size;
}

int isEmpty(struct MinHeap *heap)
{
	return heap->size == 0;
}

void dijkstra(struct Graph *graph, char source)
{
	int i, u, v;
	struct HeapNode *heapNode;
	struct AdjListNode *adjNode;

	int srcI = getIndex(graph->key, graph->size, source);

	int cost[graph->size];
	char path[graph->size];

	struct MinHeap *heap = newMinHeap(graph->size);

	for (i = 0; i < graph->size; i++)
	{
		path[i] = '-';
		cost[i] = (i == srcI) ? 0 : INT_MAX;
		insert(heap, newHeapNode(i, cost[i]));
	}

	while (!isEmpty(heap))
	{
		heapNode = extract(heap);
		u = heapNode->index;

		if (path[u] == '-' && cost[u] > 0)
			path[u] = graph->key[u];

		adjNode = graph->array[u].head;
		while (adjNode)
		{
			v = adjNode->dest;

			if (isInHeap(heap, v) && cost[u] != INT_MAX && adjNode->cost + cost[u] < cost[v])
			{
				cost[v] = cost[u] + adjNode->cost;
				path[v] = path[u];

				updateHeap(heap, v, cost[v]);
			}
			adjNode = adjNode->next;
		}
		free(heapNode);
	}

	free(heap);

	for (i = 0; i < graph->size; i++)
		if (cost[i] != INT_MAX)
			printf("%c %4d %c\n", graph->key[i], cost[i], path[i]);

	printf("\n");
}