#include "lsPacket.h"
#include "lsGraph.h"
#include "lsDijkstra.h"

int main(void)
{
	struct Graph *graph = newGraph(4, 0);

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

	// addEdge(graph, 'A', 'C', 5, 0);
	// addEdge(graph, 'C', 'A', 5, 0);
	// addEdge(graph, 'A', 'B', 2, 0);
	// addEdge(graph, 'B', 'A', 2, 0);
	// addEdge(graph, 'A', 'D', 1, 0);
	// addEdge(graph, 'D', 'A', 1, 0);
	// addEdge(graph, 'B', 'D', 2, 0);
	// addEdge(graph, 'D', 'B', 2, 0);
	// addEdge(graph, 'B', 'C', 3, 0);
	// addEdge(graph, 'C', 'B', 3, 0);
	// addEdge(graph, 'D', 'C', 3, 0);
	// addEdge(graph, 'C', 'D', 3, 0);
	// addEdge(graph, 'C', 'F', 5, 0);
	// addEdge(graph, 'F', 'C', 5, 0);
	// addEdge(graph, 'C', 'E', 1, 0);
	// addEdge(graph, 'E', 'C', 1, 0);
	// addEdge(graph, 'E', 'F', 2, 0);
	// addEdge(graph, 'F', 'E', 2, 0);
	// addEdge(graph, 'D', 'E', 1, 0);
	// addEdge(graph, 'E', 'D', 1, 0);

	printGraph(graph);
	printf("\n\n");
	dijkstra(graph, 'A');
	printf("\n");
	dijkstra(graph, 'B');
	printf("\n");
	dijkstra(graph, 'C');
	printf("\n");
	dijkstra(graph, 'D');
	printf("\n");

	return 0;
}