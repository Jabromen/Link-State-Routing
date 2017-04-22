/**
 * This file implements the functions used for the router network
 *
 * @author Jeffrey Bromen
 * @date 4/19/17
 * @info Systems and Networks II
 * @info Project 3
 */

#include "lsNetwork.h"

struct NeighborList *newNeighborList()
{
	struct NeighborList *list = (struct NeighborList *) malloc(sizeof(struct NeighborList));

	if (!list)
		return NULL;

	list->size = 0;
	list->head = NULL;

	return list;
}

struct Neighbor *newNeighbor(char label, const char *address, int port, int cost)
{
	struct Neighbor *node = (struct Neighbor *) malloc(sizeof(struct Neighbor));

	if (!node)
		return NULL;

	node->label = label;
	strcpy(node->address, address);
	node->port = port;
	node->cost = cost;

	return node;
}

void addToList(struct NeighborList *list, struct Neighbor *node)
{
	node->next = list->head;
	list->head = node;
	list->size++;
}

struct FifoQueue *newFifoQueue()
{
	struct FifoQueue *queue = (struct FifoQueue *) malloc(sizeof(struct FifoQueue));

	if (!queue)
		return NULL;

	queue->size = 0;
	queue->head = NULL;
	queue->tail = NULL;

	return queue;
}

struct QueueNode *newQueueNode(const char *packet)
{
	struct QueueNode *node = (struct QueueNode *) malloc(sizeof(struct QueueNode));

	if (!node)
		return NULL;

	memcpy(node->packet, packet, LS_PACKET_SIZE);
	node->next = NULL;

	return node;
}

void push(struct FifoQueue *queue, const char *packet)
{
	struct QueueNode *node = newQueueNode(packet);

	if (queue->size == 0)
	{
		queue->head = node;
		queue->tail = node;
	}
	else
	{
		queue->tail->next = node;
	}

	queue->size++;
}

void pop(struct FifoQueue *queue, char *buffer)
{
	if (queue->size == 0)
		return;

	struct QueueNode *node = queue->head;

	queue->head = node->next;
	queue->size--;

	if (queue->size == 0)
		queue->tail = NULL;

	memcpy(buffer, node->packet, LS_PACKET_SIZE);

	free(node);
}

int isEmptyQueue(struct FifoQueue *queue)
{
	return queue->size == 0;
}

int initializeSocket(int localPort)
{
	int fd;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("Cannot create socket");
		return -1;
	}

	struct sockaddr_in myaddr;
	
	memset((char *) &myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(localPort);
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// Bind socket
	if (bind(fd, (struct sockaddr *) &myaddr, sizeof(myaddr)) < 0) {
		perror("Bind Failed");
		return -1;
	}

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 100000;

	if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *) &tv, sizeof(struct timeval)) < 0) {
		perror("Set Socket Options Failed");
		return -1;
	}

	return fd;
}

int sendPacket(int fd, const char *packet, const char *destHost, int destPort)
{
	struct sockaddr_in destaddr;

	memset((char *)&destaddr, 0, sizeof(destaddr));
	destaddr.sin_family = AF_INET;
	inet_aton(destHost, &destaddr.sin_addr);
	destaddr.sin_port = htons(destPort);

	if (sendto(fd, packet, LS_PACKET_SIZE, 0, (struct sockaddr *)&destaddr, sizeof(destaddr)) < 0) {
		perror("Sendto failed");
		return -1;
	}

	return 0;
}

int floodPacket(int fd, const char *packet, struct NeighborList *neighbors)
{
	char *address;
	int port;
	struct Neighbor *neighbor;

	neighbor = neighbors->head;

	while (neighbor)
	{
		address = neighbor->address;
		port = neighbor->port;

		if (sendPacket(fd, packet, address, port) < 0)
			return -1;

		neighbor = neighbor->next;
	}

	return 0;
}