#include "lsPacket.h"
#include "lsGraph.h"
#include "lsDijkstra.h"

int main(void)
{
	struct graphT *graph = newGraph('D');

	addEdge(graph, 'D', 'C', 2, 0);
	addEdge(graph, 'C', 'D', 2, 0);
	addEdge(graph, 'D', 'B', 11, 0);
	addEdge(graph, 'B', 'D', 11, 0);
	addEdge(graph, 'C', 'B', 3, 0);
	addEdge(graph, 'B', 'C', 3, 0);
	addEdge(graph, 'A', 'B', 5, 0);
	addEdge(graph, 'B', 'A', 5, 0);
	addEdge(graph, 'C', 'A', 10, 0);
	addEdge(graph, 'A', 'C', 10, 0);


	// addEdgeFromPacket(graph, lsPacket);
	// printGraph(graph);
	// printf("\n");

	// buildLSPacket(lsPacket, 3, 1, 'A', 'B', 20);

	// addEdgeFromPacket(graph, lsPacket);
	printGraph(graph);

	struct routingTable *table = newRoutingTable(4);

	dijkstra(table, graph, 'D');

	printRoutingTable(table);

	return 0;
}