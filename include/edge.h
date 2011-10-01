/*
 * edge: A simple edge class
 */

#ifndef HAVE_EDGE_H
#define HAVE_EDGE_H

#include <cstdlib>
#include <iostream>
#include <cstring>

#define SUCCESS 0
#define FAILURE -1

class Edge {
 public:
	int u;
	int v;

	/* Constructor */
	Edge() {};
	Edge(int i, int j);

	/* operator= */
	void operator= (Edge e);
	/* operator== */
	bool operator== (Edge e);
	/* is_neighbour */
	bool is_neighbour (Edge e);

};

#endif
/* ! HAVE_EDGE_H */
