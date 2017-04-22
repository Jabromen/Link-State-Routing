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

struct NeighborList *newNeighborList();

struct Neighbor *newNeighbor(char label, const char *address, int port, int cost);

void addToList(struct NeighborList *list, struct Neighbor *node);

struct FifoQueue *newFifoQueue();

struct QueueNode *newQueueNode(const char *packet);

void push(struct FifoQueue *queue, const char *packet);

void pop(struct FifoQueue *queue, char *buffer);

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

int sendPacket(int fd, const char *packet, const char *destHost, int destPort);

int floodPacket(int fd, const char *packet, struct NeighborList *neighbors);

/**
 * Get the IP address of a host in dot format
 * 
 * @param buffer - buffer where the IP address string will be stored
 * @param hostname - host name of machine whose IP address is needed
 *
 * @return -1 if failed, 0 if success
 */
int getAddress(char *buffer, const char *hostname);

void processTextFile(const char *filename, struct NeighborList *neighbors);

void queueNeighbors(struct NeighborList *neighbors, struct FifoQueue *queue, char label);

#endif // _LSNETWORK_H