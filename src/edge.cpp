/*
 * edge.cpp
 */

#include "../include/edge.h"

/* Constructor */
Edge::Edge(int i, int j)
{
	u = i;
	v = j;

	return;
}

/* 
 * operator= 
 */
void Edge::operator= (Edge e)
{
	u = e.u;
	v = e.v;

	return;

} /* operator= */

/* 
 * operator==
 */
bool Edge::operator== (Edge e)
{

	return ((u == e.u) && (v == e.v));

} /* operator== */

/*
 * is_neighbour
 */
bool Edge::is_neighbour (Edge e)
{

	return ((u == e.u) || (v == e.v) ||
		(u == e.v) || (v == e.u));

} /* is_neighbour */

/* EOF */
