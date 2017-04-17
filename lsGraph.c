/**
 * This file implements the functions used for a graph structure
 *
 * @author Jeffrey Bromen
 * @date 4/16/17
 * @info Systems and Networks II
 * @info Project 3
 */

#include "lsGraph.h"

struct graphT *newGraph()
{
	struct graphT *graph = (struct graphT *) malloc(sizeof(struct graphT));

	if (!graph)
		return NULL;

	graph->vertices = NULL;

	return graph;
}

struct vertexT *newVertex(char label)
{
	struct vertexT *vertex = (struct vertexT *) malloc(sizeof(struct vertexT));

	if (!vertex)
		return NULL;

	vertex->label = label;
	vertex->edges = NULL;
	vertex->next = NULL;

	return vertex;
}

struct edgeT *newEdge(struct vertexT *to, int cost, int sequenceNum)
{
	struct edgeT *edge = (struct edgeT *) malloc(sizeof(struct edgeT));

	if (!edge)
		return NULL;

	edge->cost = cost;
	edge->sequenceNum = sequenceNum;
	edge->connectsTo = to;
	edge->next = NULL;

	return edge;
}

struct vertexT *findVertex(struct graphT *graph, char label)
{
	struct vertexT *vertex = graph->vertices;

	while (vertex)
	{
		if (vertex->label == label)
			return vertex;
		else
			vertex = vertex->next;
	}

	return NULL;
}


struct vertexT *insertNewVertex(struct graphT *graph, char label)
{
	struct vertexT *vertex = graph->vertices;

	if (!vertex)
	{
		graph->vertices = newVertex(label);
		return graph->vertices;
	}

	while (vertex->next)
		vertex = vertex->next;

	vertex->next = newVertex(label);
	return vertex->next;
}

struct edgeT *findEdge(struct vertexT *from, struct vertexT *to)
{
	struct edgeT *edge = from->edges;

	while (edge)
	{
		if (edge->connectsTo == to)
			return edge;
		else
			edge = edge->next;
	}

	return NULL;
}

struct edgeT *insertNewEdge(struct vertexT *from, struct vertexT *to, int cost, int sequenceNum)
{
	struct edgeT *edge = from->edges;

	if (!edge)
	{
		from->edges = newEdge(to, cost, sequenceNum);
		return from->edges;
	}

	while (edge->next)
		edge = edge->next;

	edge->next = newEdge(to, cost, sequenceNum);
	return edge->next;
}

int updateExistingEdge(struct edgeT *edge, int cost, int sequenceNum)
{
	if ((edge->sequenceNum + 1) % 255 != sequenceNum)
		return 1;

	edge->cost = cost;
	edge->sequenceNum = sequenceNum;
	return 0;
}

int addEdge(struct graphT *graph, char from, char to, int cost, int sequenceNum)
{
	struct vertexT *vFrom, *vTo;
	struct edgeT *edge;

	if (!(vTo = findVertex(graph, to)))
		if (!(vTo = insertNewVertex(graph, to)))
			return -1;

	if (!(vFrom = findVertex(graph, from)))
		if (!(vFrom = insertNewVertex(graph, from)))
			return -1;

	if (!(edge = findEdge(vFrom, vTo)))
	{
		if ((edge = insertNewEdge(vFrom, vTo, cost, sequenceNum)))
			return 0;
		else
			return -1;
	}
	else
		return updateExistingEdge(edge, cost, sequenceNum);
}

int processLinkStatePacket(struct graphT *graph, char *lsPacket)
{
	char from = getSourceID(lsPacket);
	char to = getDestinationID(lsPacket);
	int cost = getCost(lsPacket);
	int sequenceNum = getSequenceNumber(lsPacket);

	return addEdge(graph, from, to, cost, sequenceNum);
}

void printGraph(struct graphT *graph)
{
	struct vertexT *vertex = graph->vertices;
	struct edgeT *edge;

	while (vertex)
	{
		printf("Vertex '%c' connects to:\n", vertex->label);
		edge = vertex->edges;
		while (edge)
		{
			printf("\t'%c' at a cost of %d\n", edge->connectsTo->label, edge->cost);
			edge = edge->next;
		}
		vertex = vertex->next;
	}
}