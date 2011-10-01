/*
 * f_edge: A simple edge 
 */

#ifndef HAVE_F_EDGE_H
#define HAVE_F_EDGE_H

#include <cstdlib>
#include <iostream>
#include <cstring>
#include "../include/edge.h"

#define SUCCESS 0
#define FAILURE -1

class FEdge {
 public:
	Edge u;
	Edge v;

	/* Constructor */
	FEdge() {};
	FEdge(Edge i, Edge j);

	/* operator= */
	void operator= (FEdge e);
	/* operator== */
	bool operator== (FEdge e);
	/* is_neighbour */
	bool is_neighbour (FEdge e);

};

#endif
/* ! HAVE_F_EDGE_H */
