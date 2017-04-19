/**
 * This file implements the functions used for finding the shortest path via Dijkstra's Algorithm
 *
 * @author Jeffrey Bromen
 * @date 4/17/17
 * @info Systems and Networks II
 * @info Project 3
 */

#include "lsDijkstra.h"

void heapify(struct heapNode *heap, int count)
{
	int start = PARENT(count - 1);

	while (start >= 0)
	{
		siftDown(heap, start, count - 1);
		start--;
	}
}

void siftDown(struct heapNode *heap, int start, int end)
{
	int root, child, swp;

	root = start;

	while (LCHILD(root) <= end)
	{
		child = LCHILD(root);
		swp = root;

		if (heap[swp].value > heap[child].value)
			swp = child;

		if (child + 1 <= end && heap[swp].value > heap[child+1].value)
			swp = child + 1;

		if (swp == root)
			return;

		else
		{
			swap(&heap[root], &heap[swp]);
			root = swp;
		}
	}
}

void siftUp(struct heapNode *heap, int start)
{
	int swp, parent;

	swp = start;

	while (swp > 0)
	{
		parent = PARENT(swp);

		if (heap[parent].value > heap[swp].value)
		{
			swap(&heap[parent], &heap[swp]);
			swp = parent;
		}

		else
			return;
	}
}

void swap(struct heapNode *h1, struct heapNode *h2)
{
	int value = h1->value;
	struct tableEntry *entry = h1->entry;

	h1->value = h2->value;
	h1->entry = h2->entry;
	h2->value = value;
	h2->entry = entry;
}

struct priorityQueue *newPriorityQueue(int max)
{
	struct priorityQueue *pq = (struct priorityQueue *) malloc(sizeof(struct priorityQueue));

	if (!pq)
		return NULL;

	pq->heap = (struct heapNode *) malloc(max * sizeof(struct heapNode));
	pq->size = 0;

	if (!pq->heap)
	{
		free(pq);
		return NULL;
	}

	return pq;
}

struct heapNode newHeapNode(struct tableEntry *entry)
{
	struct heapNode node;

	node.value = entry->cost;
	node.entry = entry;

	return node;
}

void enqueue(struct priorityQueue *pq, struct tableEntry *entry)
{
	if (pq->size >= pq->max)
		pq->heap = realloc(pq->heap, (pq->size + 1) * sizeof(struct heapNode));
	
	pq->heap[pq->size++] = newHeapNode(entry);
	pq->max++;

	heapify(pq->heap, pq->size);
}

struct tableEntry *dequeue(struct priorityQueue *pq)
{
	if (pq->size <= 0)
		return NULL;

	struct tableEntry *entry = pq->heap[0].entry;

	swap(&pq->heap[0], &pq->heap[--pq->size]);

	heapify(pq->heap, pq->size);

	return entry;
}

struct routingTable *newRoutingTable(int size)
{
	struct routingTable *table = (struct routingTable *) malloc(sizeof(struct routingTable));

	if (!table)
		return NULL;

	table->table = (struct tableEntry **) malloc(size * sizeof(struct tableEntry *));

	if (!table->table)
	{
		free(table);
		return NULL;
	}

	table->size = size;
	table->numEntries = 0;

	return table;
}

struct tableEntry *newTableEntry(struct vertexT *vertex)
{
	struct tableEntry *entry = (struct tableEntry *) malloc(sizeof(struct tableEntry));

	entry->vertex = vertex;
	entry->parent = NULL;
	entry->cost = INT_MAX;

	return entry;
}

void updateQueueEntry(struct priorityQueue *pq, struct vertexT *vertex, struct vertexT *parent, char cost)
{
	int i;
	struct tableEntry *entry;
	for (i = 0; i < pq->size; i++)
	{
		if (pq->heap[i].entry->vertex == vertex)
		{
			entry = pq->heap[i].entry;
			break;
		}
	}

	if (cost < entry->cost)
	{
		entry->parent = parent;
		entry->cost = cost;
		pq->heap[i].value = cost;

		siftUp(pq->heap, i);
	}
}

int tableContains(struct routingTable *table, char vertex)
{
	int i;
	for (i = 0; i < table->numEntries; i++)
	{
		if (table->table[i]->vertex->label == vertex)
			return 1;
	}
	return 0;
}

void addEntryToTable(struct routingTable *table, struct tableEntry *entry)
{
	if (table->numEntries < table->size)
		table->table[table->numEntries++] = entry;
}

void dijkstra(struct routingTable *table, struct graphT *graph, char start)
{
	struct vertexT *vertex;
	struct edgeT *edge;
	struct tableEntry *entry;
	struct priorityQueue *pq = newPriorityQueue(table->size);

	vertex = graph->vertices;

	while (vertex)
	{
		entry = newTableEntry(vertex);

		if (vertex->label == start)
			entry->cost = 0;

		enqueue(pq, entry);
		vertex = vertex->next;
	}

	entry = dequeue(pq);
	addEntryToTable(table, entry);

	while (entry)
	{
		edge = entry->vertex->edges;

		while (edge)
		{
			if (!tableContains(table, edge->connectsTo->label))
			{
				updateQueueEntry(pq, edge->connectsTo, entry->vertex, edge->cost + entry->cost);
			}
			edge = edge->next;
		}
		entry = dequeue(pq);
		addEntryToTable(table, entry);
	}
}

void printRoutingTable(struct routingTable *table)
{
	int i;
	struct tableEntry *entry;
	for (i = 0; i < table->size; i++)
	{
		entry = table->table[i];
		if (entry->parent)
			printf("%c %4d %c\n", entry->vertex->label, entry->cost, entry->parent->label);
		else
			printf("%c %4d -\n", entry->vertex->label, entry->cost);
	}
}