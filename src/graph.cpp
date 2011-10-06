/*
 * graph.c
 */

#include "../include/graph.h"

/* Constructor */
Graph::Graph()
{
	num_vertices = 0;
	label = NULL;
	G = NULL;
	return;
}

/* Destructor */
Graph::~Graph()
{
}

/* get_num_vertices */
int Graph::get_num_vertices()
{
	return this->num_vertices;
} /* get_num_vertices */

/* set_num_vertices */
void Graph::set_num_vertices(int v)
{
	this->num_vertices = v;
	return;
} /* set_num_vertices */

/* get_vertices_array */
int *Graph::get_vertices_array()
{
	return (int *) this->label;
} /* get_vertices_array */

/* set_vertices_array */
int Graph::set_vertices_array(int V[], int n)
{
	int i;

	if (this->num_vertices != n) {
		return FAILURE;
	}
	
	this->label = new int[n];

	for (i = 0; i < n; i++) {
		this->label[i] = V[i];
	}

	return SUCCESS;
} /* set_vertices_array */

/* set_vertices_array */
int Graph::set_vertices_array(int n)
{
	int i;

	if (this->num_vertices != n) {
		return FAILURE;
	}
	
	this->label = new int[n];

	for (i = 0; i < n; i++) {
		this->label[i] = i;
	}

	return SUCCESS;
} /* set_vertices_array */

/* get_vertex_pos */
int Graph::get_vertex_pos(int v)
{
	int pos;

	for (pos = 0; pos < this->num_vertices; pos++) {
		if (this->label[pos] == v) {
			break;
		}
	}

	return pos;

} /* get_vertex_pos */

/* free_vertices_array */
void Graph::free_vertices_array()
{
	if (this->label != NULL) {
		delete[] this->label;
	}

	return;
} /* free_vertices_array */

/* create_adj_matrix */
void Graph::create_adj_matrix()
{
	int i, j;

	this->G = (int **) malloc(this->num_vertices*sizeof(int *));

	for (i = 0; i < this->num_vertices; i++) {
		this->G[i] = (int *) malloc(this->num_vertices*
					    sizeof(int));
		for (j = 0; j < this->num_vertices; j++) {
			this->G[i][j] = 0;
		}
	}

	return;
} /* create_adj_matrix */

/* get_adj_matrix */
int **Graph::get_adj_matrix()
{
	return (int **) this->G;
} /* get_adj_matrix */

/* add_edge */
void Graph::add_edge(int u, int v, int color)
{
	int i, j;

	if (this->G == NULL) {
		this->create_adj_matrix();
	}

	for (i = 0; i < this->num_vertices; i++) {
		if (this->label[i] == u) {
			break;
		}
	}

	for (j = 0; j < this->num_vertices; j++) {
		if (this->label[j] == v) {
			break;
		}
	}

	this->G[i][j] = color;

	return;
} /* add_edge */

/* free_adj_matrix */
void Graph::free_adj_matrix()
{
	int i;

	if (this->G != NULL) {

		for (i = 0; i < this->num_vertices; i++) {
			free(this->G[i]);
		}
		free(this->G);
	}

	return;
} /* free_adj_matrix */

/* EOF */
