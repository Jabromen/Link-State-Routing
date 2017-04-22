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
	if (argc < 5)
	{
		fprintf(stderr, "Not enough arguments. Use format:\n"
		                "routerLabel portNum totalNumRouters discoverFile [-dynamic]\n");
		exit(EXIT_FAILURE);
	}

	char label = argv[1][0];
	int port = atoi(argv[2]);
	int numRouters = atoi(argv[3]);
	char *filename = argv[4];

	int err, fd, uptodate;

	char recvBuffer[LS_PACKET_SIZE];

	if ((fd = initializeSocket(port)) < 0)
		exit(EXIT_FAILURE);

	pthread_t network_thread;

	struct Graph *graph = newGraph(numRouters, 0);

	neighbors = newNeighborList();

	sendQueue = newFifoQueue();
	recvQueue = newFifoQueue();

	if(sem_init(&sendLock, 0, 1) != 0) {
		printf("Sem init failed.\n");
		exit(EXIT_FAILURE);
	}

	if(sem_init(&recvLock, 0, 1) != 0) {
		printf("Sem init failed.\n");
		exit(EXIT_FAILURE);
	}

	/**
	 * The text file processing should populate the neighbor list
	 * and build link-state packets of the initial info and push them
	 * into the recvQueue to be processed/sent later in the below while loop.
	 */
	// process_text_file(neighbors, recvQueue, filename);

	if ((err = pthread_create(&network_thread, NULL, &networkThread, &fd))) {
		fprintf(stderr, "Can't create Network Thread: [%s]\n", strerror(err));
		exit(EXIT_FAILURE);
	}

	uptodate = 0;

	while (1)
	{
		// Process recveived packet, decrement hop count, and push to send queue
		while (!isEmptyQueue(recvQueue))
		{
			sem_wait(&recvLock);

			pop(recvQueue, recvBuffer);

			sem_post(&recvLock);

			addEdgeFromPacket(graph, recvBuffer);

			if (decrementHopCount(recvBuffer) > 0)
			{
				sem_wait(&sendLock);

				push(sendQueue, recvBuffer);

				sem_post(&sendLock);
			}
			uptodate = 0;
		}
		// Give time to receive new packets and delay forwarding table calculations
		sleep(1);

		if (isEmptyQueue(recvQueue) && !uptodate)
		{
			dijkstra(graph, label);
			uptodate = 1;
		}
	}
}

void *networkThread(void *param)
{
	int fd = *((int *) param);

	int recvLen;

	char sendBuffer[LS_PACKET_SIZE];
	char recvBuffer[LS_PACKET_SIZE];

	while (1)
	{
		while (!isEmptyQueue(sendQueue))
		{
			sem_wait(&sendLock);

			pop(sendQueue, sendBuffer);

			sem_post(&sendLock);

			floodPacket(fd, sendBuffer, neighbors);
		}

		recvLen = recv(fd, recvBuffer, LS_PACKET_SIZE, 0);

		if (recvLen > 0)
		{
			sem_wait(&recvLock);

			push(recvQueue, recvBuffer);

			sem_post(&recvLock);
		}
	}
}