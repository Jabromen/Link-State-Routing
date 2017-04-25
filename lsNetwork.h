/**
 * This file describes the functions used for the router network
 *
 * @author Jeffrey Bromen
 * @date 4/19/17
 * @info Systems and Networks II
 * @info Project 3
 */

#ifndef _LSNETWORK_H
#define _LSNETWORK_H

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "lsDijkstra.h"
#include "lsGraph.h"
#include "lsPacket.h"

#define DELIM ","

struct NeighborList
{
	int size;
	struct Neighbor *head;
} NeighborList;

struct Neighbor
{
	char label;
	char address[INET_ADDRSTRLEN];
	int port;
	int cost;
	struct Neighbor *next;
} Neighbor;

struct FifoQueue
{
	int size;
	struct QueueNode *head;
	struct QueueNode *tail;
} FifoQueue;

struct QueueNode
{
	char packet[LS_PACKET_SIZE];
	struct QueueNode *next;
} QueueNode;

/**
 * Initializes a new neighbor list structure.
 *
 * @return - pointer to list structure
 */
struct NeighborList *newNeighborList();

/**
 * Initializes a new neighbor list node structure.
 *
 * @param label   - label of neighboring router
 * @param address - network address of neighboring router
 * @param port    - port number of neighboring router
 * @param cost    - cost to reach neighboring router
 *
 * @return - pointer to list node
 */
struct Neighbor *newNeighbor(char label, const char *address, int port, int cost);

/**
 * Adds a neighbor node to a neighbor list.
 *
 * @param list - neighbor list
 * @param node - neighbor node being added
 */
void addToList(struct NeighborList *list, struct Neighbor *node);

/**
 * Initializes a new FIFO queue.
 *
 * @return - pointer to queue
 */
struct FifoQueue *newFifoQueue();

/**
 * Initializes a new queue node.
 *
 * @param packet - packet being stored in node
 *
 * @return - pointer to node
 */
struct QueueNode *newQueueNode(const char *packet);

/**
 * Pushes a packet into a FIFO queue
 *
 * @param queue  - FIFO queue
 * @param packet - packet being pushed to queue
 */
void push(struct FifoQueue *queue, const char *packet);

/**
 * Pops a packet from a FIFO queue
 *
 * @param queue  - FIFO queue
 * @param buffer - buffer where popped packet will be stored
 */
void pop(struct FifoQueue *queue, char *buffer);

/**
 * Check if FIFO queue is empty
 *
 * @param queue - FIFO queue
 *
 * @return - 1 if empty, 0 if not empty
 */
int isEmptyQueue(struct FifoQueue *queue);

/**
 * Initializes and binds a UDP socket
 *
 * @param localPort - port number of socket being opened
 * @param sender    - 1 if sender timeout option is needed, 0 otherwise
 *
 * @return file descriptor for socket if success, -1 if failure
 */
int initializeSocket(int localPort);

/**
 * Sends a packet to a specified destination.
 *
 * @param fd       - file descriptor of socket being used
 * @param packet   - packet being sent
 * @param destHost - network address of destination
 * @param destPort - port number of destination
 *
 * @return - 0 if successful, -1 if error occurred
 */
int sendPacket(int fd, const char *packet, const char *destHost, int destPort);

/**
 * Sends a packet to all of the neighboring routers.
 *
 * @param fd        - file descriptor of socket being used
 * @param packet    - packet being sent
 * @param neighbors - list of neighboring routers
 *
 */
int floodPacket(int fd, const char *packet, struct NeighborList *neighbors);

/**
 * Get the IP address of a host in dot format
 * 
 * @param buffer   - buffer where the IP address string will be stored
 * @param hostname - host name of machine whose IP address is needed
 *
 * @return -1 if failed, 0 if success
 */
int getAddress(char *buffer, const char *hostname);

/**
 * Reads a text file to discover the neighboring routers.
 *
 * @param filename  - filename of discovery text file
 * @param neighbors - neighbor list structure
 *
 * @return - 0 if success, -1 if error
 */
int processTextFile(const char *filename, struct NeighborList *neighbors);

/**
 * Queues all of the neighboring router edges.
 *
 * @param neighbors - neighbor list
 * @param queue     - FIFO queue
 * @param label     - label of router
 */
void queueNeighbors(struct NeighborList *neighbors, struct FifoQueue *queue, char label);

#endif // _LSNETWORK_H