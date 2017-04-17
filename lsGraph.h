/**
 * This file describes the functions used for a graph structure
 *
 * @author Jeffrey Bromen
 * @date 4/16/17
 * @info Systems and Networks II
 * @info Project 3
 */

#ifndef _LSGRAPH_H
#define _LSGRAPH_H

#include <stdio.h>
#include <stdlib.h>

#include "lsPacket.h"

struct graphT {
	struct vertexT *vertices;
} graphT;

struct vertexT {
	char label;
	struct edgeT *edges;
	struct vertexT *next;
} vertexT;

struct edgeT {
	int cost;
	int sequenceNum;
	struct vertexT *connectsTo;
	struct edgeT *next;
} edgeT;

/**
 * Allocates memory for a new graph structure.
 *
 * @return - pointer to graph structure
 */
struct graphT *newGraph();

/**
 * Allocates memory for a new vertex structure.
 *
 * @param label - label of vertex
 *
 * @return - pointer to vertex structure
 */
struct vertexT *newVertex(char label);

/**
 * Allocates memory for a new edge structure.
 *
 * @param cost        - cost of traversing the edge
 * @param sequenceNum - sequence number of last link-state packet received.
 *
 * @return - pointer to edge structure
 */
struct edgeT *newEdge(struct vertexT *to, int cost, int sequenceNum);

struct vertexT *findVertex(struct graphT *graph, char label);

struct vertexT *insertNewVertex(struct graphT *graph, char label);

/**
 * Adds an edge to the graph structure.
 *
 * @param graph       - graph structure being modified
 * @param from        - vertex label of source router
 * @param to          - vertex label of destination router
 * @param cost        - cost of traversing edge
 * @param sequenceNum - sequence number of link-state packet
 *
 * @return - 0 if successful, -1 if an error occurred
 */
int addEdge(struct graphT *graph, char from, char to, int cost, int sequenceNum);

/**
 * Processes a link state packet and adds/modifies the graph accordingly
 *
 * @param graph - graph structure of router network
 * @param lsPacket - link-state packet received
 *
 * @return - 0 if successful, -1 if an error occurred
 */
int processLinkStatePacket(struct graphT *graph, char *lsPacket);

void printGraph(struct graphT *graph);

#endif // _LSGRAPH_H