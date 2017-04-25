/**
 * This file implements the functions used for a graph structure
 *
 * @author Jeffrey Bromen
 * @date 4/17/17
 * @info Systems and Networks II
 * @info Project 3
 */

#include "lsGraph.h"

struct Graph *newGraph(int size, int directed)
{
	int i;
	struct Graph *graph = (struct Graph *) malloc(sizeof(struct Graph));

	if (!graph)
		return NULL;

	if(!(graph->key = (char *) malloc(size * sizeof(char))))
	{
		free(graph);
		return NULL;
	}

	for (i = 0; i < size; i++)
		graph->key[i] = '\0';

	if(!(graph->array = (struct AdjList *) malloc(size * sizeof(struct AdjList))))
	{
		free(graph->key);
		free(graph);
		return NULL;
	}

	for (i = 0; i < size; i++)
		graph->array[i].head = NULL;

	graph->size = size;
	graph->directed = directed;
	graph->updated = 0;

	return graph;
}

struct AdjListNode *newAdjListNode(int dest, int cost, int seqN)
{
	struct AdjListNode *node = (struct AdjListNode *) malloc(sizeof(struct AdjListNode));

	if (!node)
		return NULL;

	node->dest = dest;
	node->cost = cost;
	node->seqN = seqN;
	node->next = NULL;

	return node;
}

int updateEdge(struct Graph *graph, int source, int dest, int cost, int seqN)
{
	int returnVal = 0;
	struct AdjListNode *node = graph->array[source].head;

	while (node)
	{
		if (node->dest == dest)
		{
			if ((node->seqN + 1) % 256 == seqN)
			{
				node->cost = cost;
				node->seqN = seqN;
				graph->updated = 1;
			}
			returnVal = 1;
			break;
		}
		node = node->next;
	}

	// If undirected graph, find and update reverse edge as well
	if (!graph->directed && returnVal)
	{
		node = graph->array[dest].head;

		while (node)
		{
			if (node->dest == source)
			{
				if ((node->seqN + 1) % 256 == seqN)
				{
					node->cost = cost;
					node->seqN = seqN;
				}
				break;
			}
			node = node->next;
		}
	}
	return returnVal;
}

int addEdge(struct Graph *graph, char source, char dest, int cost, int seqN)
{
	int srcI, destI;
	struct AdjListNode *node;

	// Get the indices of the source and destination nodes
	srcI = getIndex(graph->key, graph->size, source);
	destI = getIndex(graph->key, graph->size, dest);

	if (srcI < 0 || destI < 0)
		return 0;

	// Attempt to update existing edge
	if (updateEdge(graph, srcI, destI, cost, seqN))
		return 1;

	// If no existing edge was found, add a new edge
	if (!(node = newAdjListNode(destI, cost, seqN)))	
		return 0;

	node->next = graph->array[srcI].head;
	graph->array[srcI].head = node;
	graph->updated = 1;

	// If undirected graph, add reverse edge as well
	if (!graph->directed)
	{
		if (!(node = newAdjListNode(srcI, cost, seqN)))
			return 0;

		node->next = graph->array[destI].head;
		graph->array[destI].head = node;
	}

	return 1;
}

int getIndex(char *key, int size, char label)
{
	int i;

	for (i = 0; i < size && key[i]; i++)
	{
		if (label == key[i])
			return i;
	}

	if (i < size)
	{
		key[i] = label;
		return i;
	}

	return -1;
}

int addEdgeFromPacket(struct Graph *graph, char *lsPacket)
{
	char source = getSourceID(lsPacket);
	char dest = getDestinationID(lsPacket);
	int cost = getCost(lsPacket);
	int seqN = getSequenceNumber(lsPacket);

	return addEdge(graph, source, dest, cost, seqN);
}

void printGraph(struct Graph *graph)
{
	int i;
	struct AdjListNode *node;

	for (i = 0; i < graph->size; i++)
	{
		node = graph->array[i].head;

		printf("Vertex '%c' connects to:\n", graph->key[i]);

		while (node)
		{
			printf("\t'%c' at a cost of %d\n", graph->key[node->dest], node->cost);
			node = node->next;
		}
	}
}