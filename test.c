#include "lsPacket.h"
#include "lsGraph.h"

int main(void)
{
	char lsPacket[LS_PACKET_SIZE];

	buildLSPacket(lsPacket, 3, 0, 'A', 'B', 20);

	printLSPacket(lsPacket);

	int hopCount = getHopCount(lsPacket);

	while (hopCount > 0)
	{
		hopCount = decrementHopCount(lsPacket);
		printLSPacket(lsPacket);
	}

	struct graphT *graph = newGraph();

	addEdge(graph, 'A', 'B', 60, 0);
	addEdge(graph, 'A', 'C', 50, 0);
	addEdge(graph, 'A', 'D', 40, 0);
	addEdge(graph, 'A', 'E', 30, 0);

	processLinkStatePacket(graph, lsPacket);
	printGraph(graph);
	printf("\n");

	buildLSPacket(lsPacket, 3, 1, 'A', 'B', 20);

	processLinkStatePacket(graph, lsPacket);
	printGraph(graph);

	return 0;
}