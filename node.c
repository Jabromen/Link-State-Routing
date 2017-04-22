/**
 * This file implements the main process for a router node.
 *
 * @author Jeffrey Bromen
 * @date 4/21/17
 * @info Systems and Networks II
 * @info Project 3
 */

#include <pthread.h>

#ifdef __APPLE__
#include <mach/semaphore.h>
#include <mach/task.h>
#define sem_init(a,b,c)     semaphore_create(mach_task_self(), (semaphore_t *)a, SYNC_POLICY_FIFO, c)
#define sem_destroy(a)      semaphore_destroy(mach_task_self(), *((semaphore_t *)a))
#define sem_post(a)         semaphore_signal(*((semaphore_t *)a))
#define sem_wait(a)         semaphore_wait(*((semaphore_t *)a))
#define sem_t               semaphore_t
#else
#include <semaphore.h>
#endif

#include "lsNetwork.h"
#include "lsPacket.h"
#include "lsGraph.h"
#include "lsDijkstra.h"

struct NeighborList *neighbors;

struct FifoQueue *sendQueue;
struct FifoQueue *recvQueue;

sem_t sendLock;
sem_t recvLock;

void *networkThread(void *param);

int main(int argc, char **argv)
{
	int err, fd, port, numRouters;
	char label, *filename;
	char recvBuffer[LS_PACKET_SIZE];
	struct Graph *graph;
	pthread_t network_thread;

	if (argc < 5) {
		fprintf(stderr, "Not enough arguments. Use format:\n"
		                "routerLabel portNum totalNumRouters discoverFile [-dynamic]\n");
		exit(EXIT_FAILURE);
	}

	// Read command line arguments
	label = argv[1][0];
	port = atoi(argv[2]);
	numRouters = atoi(argv[3]);
	filename = argv[4];

	// Create and bind socket
	if ((fd = initializeSocket(port)) < 0)
		exit(EXIT_FAILURE);

	// Initialize data structures
	graph = newGraph(numRouters, 0);
	neighbors = newNeighborList();
	sendQueue = newFifoQueue();
	recvQueue = newFifoQueue();

	if (!graph || !neighbors || !sendQueue || !recvQueue) {
		printf("Malloc failed.\n");
		exit(EXIT_FAILURE);
	}

	// Initialize semaphores
	if(sem_init(&sendLock, 0, 1) != 0) {
		printf("Sem init failed.\n");
		exit(EXIT_FAILURE);
	}
	if(sem_init(&recvLock, 0, 1) != 0) {
		printf("Sem init failed.\n");
		exit(EXIT_FAILURE);
	}

	// Read discovery text file to find adjacent neighbor nodes
	processTextFile(filename, neighbors);
	// Push neighbor edges to received queue to be processed in main loop
	queueNeighbors(neighbors, recvQueue, label);

	// Start network thread
	if ((err = pthread_create(&network_thread, NULL, &networkThread, &fd))) {
		fprintf(stderr, "Can't create Network Thread: [%s]\n", strerror(err));
		exit(EXIT_FAILURE);
	}
	// Initialization finished, give other processes time to set up before continuing
	printf("Press any key to continue when all nodes are running.\n");
	getchar();

	// The main loop where all the processing occurs
	while (1)
	{
		// Process recveived packets in received queue until empty
		while (!isEmptyQueue(recvQueue))
		{
			sem_wait(&recvLock);
			// Pop packet from queue
			pop(recvQueue, recvBuffer);

			sem_post(&recvLock);
			// Update the graph
			addEdgeFromPacket(graph, recvBuffer);
			// If hop count greater than 0 after decrementing:
			if (decrementHopCount(recvBuffer) > 0)
			{
				sem_wait(&sendLock);
				// Push packet to send queue to be sent on network thread
				push(sendQueue, recvBuffer);

				sem_post(&sendLock);
			}
		}
		// If all received packets are processed and the graph 
		// has been changed since the last shortest path calculation:
		if (isEmptyQueue(recvQueue) && graph->updated)
		{
			// Calculate the shortest path and print the forwarding table
			dijkstra(graph, label);
		}
	}
}

void *networkThread(void *param)
{
	int fd = *((int *) param);

	int recvLen;

	char sendBuffer[LS_PACKET_SIZE];
	char recvBuffer[LS_PACKET_SIZE];

	// Main loop where the network thread behavior is determined
	while (1)
	{
		// Forward packets in send queue until queue is empty
		while (!isEmptyQueue(sendQueue))
		{
			sem_wait(&sendLock);
			// Pop packet from send queue
			pop(sendQueue, sendBuffer);

			sem_post(&sendLock);
			// Send packet to all adjacent neighbors
			floodPacket(fd, sendBuffer, neighbors);
		}
		// Receive packet (time out set on socket receive operation)
		recvLen = recv(fd, recvBuffer, LS_PACKET_SIZE, 0);
		// If packet was received:
		if (recvLen > 0)
		{
			sem_wait(&recvLock);
			// Push packet to received queue to be processed in main thread
			push(recvQueue, recvBuffer);

			sem_post(&recvLock);
		}
	}
}