/**
 * This file describes the functions used for a graph structure
 *
 * @author Jeffrey Bromen
 * @date 4/17/17
 * @info Systems and Networks II
 * @info Project 3
 */

#ifndef _LSGRAPH_H
#define _LSGRAPH_H

#include <limits.h>
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
 * @param label - label of root vertex of graph
 *
 * @return - pointer to graph structure
 */
struct graphT *newGraph(char label);

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

/**
 * Finds a vertex with a given label in a graph.
 *
 * @param graph - graph structure
 * @param label - vertex label being searched for
 *
 * @return - pointer to vertex if found, NULL if not found
 */
struct vertexT *findVertex(struct graphT *graph, char label);

/**
 * Inserts a new vertex into a graph
 *
 * @param graph - graph structure
 * @param label - label of vertex being inserted
 *
 * @return - pointer to added vertex
 */
struct vertexT *insertNewVertex(struct graphT *graph, char label);

/**
 * Finds a particular edge of a vertex
 *
 * @param from - vertex from which the edge originates
 * @param to   - vertex to which the edge terminates
 *
 * @return - pointer to edge if found, NULL if not found
 */
struct edgeT *findEdge(struct vertexT *from, struct vertexT *to);

/**
 * Inserts a new edge into a vertex
 *
 * @param from        - vertex from which the edge originates
 * @param to          - vertex to which the edge terminates
 * @param cost        - cost of traversing edge
 * @param sequenceNum - sequence number of the link-state packet
 *
 * @return - pointer to added edge
 */
struct edgeT *insertNewEdge(struct vertexT *from, struct vertexT *to, int cost, int sequenceNum);

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
 * Updates an existing edge if a newer sequence number is received.
 *
 * @param edge        - edge being modified
 * @param cost        - new cost of edge
 * @param sequenceNum - sequence number of received link-state packet
 *
 * @return - 1 if not updated, 0 if updated
 */
int updateExistingEdge(struct edgeT *edge, int cost, int sequenceNum);

/**
 * Processes a link state packet and adds/modifies the graph accordingly
 *
 * @param graph - graph structure of router network
 * @param lsPacket - link-state packet received
 *
 * @return - 0 if successful, -1 if an error occurred
 */
int addEdgeFromPacket(struct graphT *graph, char *lsPacket);

/**
 * Prints all of the vertices and edges of a graph. Used for debugging.
 *
 * @param graph - graph being printed
 */
void printGraph(struct graphT *graph);

#endif // _LSGRAPH_H