/*
 * f_edge.cpp
 */

#include "../include/f_edge.h"

/* Constructor */
FEdge::FEdge(Edge i, Edge j)
{
	u = i;
	v = j;

	return;
}

/* 
 * operator= 
 */
void FEdge::operator= (FEdge e)
{
	u = e.u;
	v = e.v;

	return;

} /* operator= */

/* 
 * operator==
 */
bool FEdge::operator== (FEdge e)
{
	return ((u == e.u) && (v == e.v));

} /* operator== */

/*
 * is_neighbour
 */
bool FEdge::is_neighbour (FEdge e)
{

	return ((u == e.u) || (v == e.v) ||
		(u == e.v) || (v == e.u));

} /* is_neighbour */

/* EOF */
