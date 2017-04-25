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

/**
 * Thread function for managing incoming and outgoing link-state packets.
 *
 * @param param - integer pointer to socket file descriptor
 */
void *networkThread(void *param);
/**
 * Thread function for periodically changing a neighboring edge cost.
 *
 * @param param - character pointer to local router label
 */
void *dynamicThread(void *param);

/**
 * Parses the command line arguments and stores the results in the parameters 
 *
 * @param argc      - number of arguments
 * @param argv      - argument vector
 * @param label     - label of local router
 * @param port      - local port number
 * @param numRouter - number of routers in the network
 * @param filename  - file name of neighbor discovery file
 * @param dynamic   - flag indicating whether dynamic option was enabled
 *
 * @return - 0 if success, -1 if error
 */
int parseCommandLine(int argc, char **argv, char *label, int *port, int *numRouters, char **filename, int *dynamic);
/**
 * Initializes the socket and data structures that are used in the program.
 *
 * @param fd         - socket file descriptor
 * @param port       - local port number
 * @param numRouters - number of routers in the network
 * @param filename   - file name of neighbor discovery file
 * @param label      - label of local router
 *
 * @return - 0 if success, -1 if error
 */
int initialization(int *fd, int port, int numRouters, char *filename, char label);
/**
 * Creates and starts the network thread.
 *
 * @param fd - socket file descriptor
 *
 * @return - 0 if success, -1 if error
 */
int startNetworkThread(int *fd);
/**
 * Creates and starts the dynamic thread.
 *
 * @param label - label of local router
 *
 * @return - 0 if success, -1 if error
 */
int startDynamicThread(char *label);

// Graph of all nodes and edges in the network
struct Graph *graph;
// List containing the neighbor info read from file
struct NeighborList *neighbors;
// Queue containing packets waiting to be sent
struct FifoQueue *sendQueue;
// Queue containing packets waiting to be processed
struct FifoQueue *recvQueue;

// Semaphores for synchronizing threads
sem_t sendLock;
sem_t recvLock;
sem_t dynamLock;

int main(int argc, char **argv)
{
	int fd, port, numRouters, dynamic, dLock;
	char label, *filename;
	char recvBuffer[LS_PACKET_SIZE];

	// Parse command line arguments to get parameters and set dynamic flag
	if (parseCommandLine(argc, argv, &label, &port, &numRouters, &filename, &dynamic) < 0)
		exit(EXIT_FAILURE);

	// Initialize socket and data structures
	if (initialization(&fd, port, numRouters, filename, label) < 0)
		exit(EXIT_FAILURE);

	// Start network thread
	if (startNetworkThread(&fd) < 0)
		exit(EXIT_FAILURE);
	
	// Initialization finished, give other processes time to set up before continuing
	printf("Press enter to continue after all nodes are running.\n");
	getchar();

	// Start dynamic change thread if dynamic flag is set
	if (dynamic)
	{
		if (startDynamicThread(&label) < 0)
			exit(EXIT_FAILURE);
		dLock = 1;
	}
	else
		dLock = 0;

	// The main loop where all the processing occurs
	while (1)
	{
		// Process recveived packets in received queue until empty
		while (!isEmptyQueue(recvQueue))	
		{
			// Pop packet from queue
			sem_wait(&recvLock);
			pop(recvQueue, recvBuffer);
			sem_post(&recvLock);
			// Update the graph
			addEdgeFromPacket(graph, recvBuffer);
			// If hop count greater than 0 after decrementing:
			if (decrementHopCount(recvBuffer) > 0)
			{
				// Push packet to send queue to be sent on network thread
				sem_wait(&sendLock);
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
			// If dynamic thread is initially blocked, allow it to continue
			if (dLock) 
			{
				sem_post(&dynamLock);
				dLock = 0;
			}
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

void *dynamicThread(void *param)
{
	char label;
	char packet[LS_PACKET_SIZE];
	int i, num, numEdges, cost;
	struct AdjList vertex;
	struct AdjListNode *edge;

	label = *((char *) param);
	numEdges = neighbors->size;

	// Block until first shortest path calculation
	sem_wait(&dynamLock);
	// Seed the random number generator
	srand(time(NULL));
	// Get the vertex of the local router
	i = getIndex(graph->key, graph->size, label);
	vertex = graph->array[i];

	// Loop handling the creation of dynamic network changes
	while (1)
	{
		// Change the cost of a random edge of the router every 5 seconds
		sleep(5);
		// Pick a random edge
		num = rand() % (numEdges);
		// Loop to the picked edge
		i = 0;
		edge = vertex.head;
		while (edge && i < num)
		{
			edge = edge->next;
			i++;
		}

		if (edge)
		{
			// Add a random number between -4 and +4 to get the new cost
			cost = edge->cost + (rand() % 9) - 4;
			// If new cost is less than 1, set to 1
			cost = cost < 1 ? 1 : cost;
			// Build link-state packet to enact change to graph
			buildLSPacket(packet, 6, (edge->seqN + 1) % 256, label, graph->key[edge->dest], cost);
			// Display changes
			printf("Changing cost to reach %c from %d to %d\n", graph->key[edge->dest], edge->cost, cost);
			// Push packet onto queue to be processed
			sem_wait(&recvLock);
			push(recvQueue, packet);
			sem_post(&recvLock);
		}
	}
}

int parseCommandLine(int argc, char **argv, char *label, int *port, int *numRouters, char **filename, int *dynamic)
{
	if (argc < 5) {
		fprintf(stderr, "Not enough arguments. Use format:\n"
		                "routerLabel portNum totalNumRouters discoverFile [-dynamic]\n");
		return -1;
	}

	// Read command line arguments
	*label = argv[1][0];
	*port = atoi(argv[2]);
	*numRouters = atoi(argv[3]);
	*filename = argv[4];

	// Set dynamic flag
	if (argc >= 6 && !strcmp(argv[5], "-dynamic\0"))
		*dynamic = 1;
	else
		*dynamic = 0;

	return 0;
}

int initialization(int *fd, int port, int numRouters, char *filename, char label)
{
	// Create and bind socket
	if ((*fd = initializeSocket(port)) < 0)
		return -1;

	// Initialize data structures
	graph = newGraph(numRouters, 0);
	neighbors = newNeighborList();
	sendQueue = newFifoQueue();
	recvQueue = newFifoQueue();

	if (!graph || !neighbors || !sendQueue || !recvQueue) {
		printf("Malloc failed.\n");
		return -1;
	}

	// Initialize semaphores
	if(sem_init(&sendLock, 0, 1) != 0) {
		printf("Sem init failed.\n");
		return -1;
	}
	if(sem_init(&recvLock, 0, 1) != 0) {
		printf("Sem init failed.\n");
		return -1;
	}

	// Read discovery text file to find adjacent neighbor nodes
	if (processTextFile(filename, neighbors) < 0)
		return -1;
	// Push neighbor edges to received queue to be processed in main loop
	queueNeighbors(neighbors, recvQueue, label);

	return 0;
}

int startNetworkThread(int *fd)
{
	int err;
	pthread_t network_thread;
	// Start network thread
	if ((err = pthread_create(&network_thread, NULL, &networkThread, fd))) {
		fprintf(stderr, "Can't create Network Thread: [%s]\n", strerror(err));
		return -1;
	}

	return 0;
}

int startDynamicThread(char *label)
{
	int err;
	pthread_t dynamic_thread;

	if(sem_init(&dynamLock, 0, 1) != 0) {
		printf("Sem init failed.\n");
		return -1;
	}
	// Block dynamic thread until after first shortest path calculation
	sem_wait(&dynamLock);

	// Start dynamic thread
	if ((err = pthread_create(&dynamic_thread, NULL, &dynamicThread, label))) {
		fprintf(stderr, "Can't create Dynamic Thread: [%s]\n", strerror(err));
		return -1;
	}

	return 0;
}