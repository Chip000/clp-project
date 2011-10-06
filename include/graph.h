/*
 * graph.h A simple graph class
 */

#ifndef HAVE_GRAPH_H
#define HAVE_GRAPH_H

#include <cstdlib>
#include <iostream>
#include <cstring>

#define SUCCESS 0
#define FAILURE -1

class Graph {
 protected:
	int num_vertices;
	int *label;
	int **G;
 public:
	/* Constructor */
	Graph();
	/* Destructor */
	~Graph();

	/* get_num_vertices */
	int get_num_vertices();
	/* set_num_vertices */
	void set_num_vertices(int v);
	/* get_vertices_array */
	int *get_vertices_array();
	/* set_vertices_array */
	int set_vertices_array(int V[], int n);
	/* set_vertices_array */
	int set_vertices_array(int n);
	/* get_vertex_pos */
	int get_vertex_pos(int v);
	/* free_vertices_array */
	void free_vertices_array();
	/* create_adj_matrix */
	void create_adj_matrix();
	/* get_adj_matrix */
	int **get_adj_matrix();
	/* add_edge */
	void add_edge(int u, int v, int color);
	/* free_adj_matrix */
	void free_adj_matrix();
};

#endif
/* ! HAVE_GRAPH_H */
