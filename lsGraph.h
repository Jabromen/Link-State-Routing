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

struct Graph
{
	int size;
	int directed;
	int updated;
	char *key;
	struct AdjList *array;
} Graph;

struct AdjList
{
	struct AdjListNode *head;
} AdjList;

struct AdjListNode
{
	int dest;
	int cost;
	int seqN;
	struct AdjListNode *next;
} AdjListNode;

/**
 * Allocates memory for a new graph structure.
 *
 * @param size     - number of nodes in graph
 * @param directed - 1 if directed graph, 0 if undirected graph
 *
 * @return - pointer to graph structure
 */
struct Graph *newGraph(int size, int directed);

/**
 * Allocates memory for a new adjacency list node
 *
 * @param dest - index of destination node
 * @param cost - cost of traversing the edge
 * @param seqN - sequence number of last link-state packet received.
 *
 * @return - pointer to edge structure
 */
struct AdjListNode *newAdjListNode(int dest, int cost, int seqN);

/**
 * Adds or updates an edge in a graph structure.
 *
 * @param graph  - graph structure being modified
 * @param source - node label of source
 * @param dest   - node label of destination
 * @param cost   - cost of traversing edge
 * @param seqN   - sequence number of link-state packet
 *
 * @return - 0 if successful, -1 if an error occurred
 */
int addEdge(struct Graph *graph, char source, char dest, int cost, int seqN);

/**
 * Updates an existing edge if a newer sequence number is received.
 *
 * @param graph  - graph being updated
 * @param source - index of source node
 * @param dest   - index of destination node
 * @param cost   - new cost of edge
 * @param seqN   - sequence number of received link-state packet
 *
 * @return - 1 if not updated, 0 if updated
 */
int updateEdge(struct Graph *graph, int source, int dest, int cost, int seqN);

/**
 * Finds the index of a router label.
 * Assigns a label to an index if not found and room is availible.
 *
 * @param key   - array of router labels
 * @param size  - size of array
 * @param label - label being searched for
 *
 * @return - index of label, -1 if not found and unable to assign index
 */
int getIndex(char *key, int size, char label);

/**
 * Processes a link state packet and adds/modifies the graph accordingly
 *
 * @param graph - graph structure of router network
 * @param lsPacket - link-state packet received
 *
 * @return - 0 if successful, -1 if an error occurred
 */
int addEdgeFromPacket(struct Graph *graph, char *lsPacket);

/**
 * Prints all of the vertices and edges of a graph. Used for debugging.
 *
 * @param graph - graph being printed
 */
void printGraph(struct Graph *graph);

#endif // _LSGRAPH_H