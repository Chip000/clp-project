/*
 * prob.cpp
 */

#include "../include/prob.h"
//#include "../include/mylist.h"
#include <list>
#include <cassert>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/max_cardinality_matching.hpp>

#define EDGE 1
#define GRAY 1
#define BLACK 2

using namespace boost;
typedef adjacency_list<vecS, vecS, undirectedS> my_graph;

/*
 * count_odd_cycles
 */
int count_odd_cycles(int **M, int n)
{
	int i, j;
	int c = 0;
	int *visited = new int[n];
	int u;
	int codd = 0;

	memset((void *) visited, 0, n);

	for (i = 0; i < n; i++) {
		if (visited[i] != 0) {
			continue;
		}

		u = i;
		j = 0;
		while (j < n) {
			if (M[u][j] != 0) {
				if (visited[u] != 0) {
					break;
				} 
				if (M[u][j] == BLACK) {
					c ++;
				}
				visited[u] = 1;
				u = j;
				j = -1;
			}
			j++;
		}

		if (c % 2 != 0) {
			codd++;
		}
		c = 0;
	}

	delete[] visited;

	return codd;

} /* count_odd_cycles */

/*
 * find_gray_edges
 */
int find_gray_edges(Graph G, Edge u, Edge v)
{
	int ret;

	ret = 0;

	int **M = G.get_adj_matrix();

	if (((M[u.u][v.u] == GRAY) && (M[u.v][v.v] == GRAY)) ||
	    ((M[u.v][v.u] == GRAY) && (M[u.u][v.v] == GRAY))) {
		ret = 1;
	}

	return ret;

} /* find_gray_edges */

/*
 * get_gray_edges
 */
int get_gray_edges(Graph G, Edge u, Edge v, FEdge &E)
{
	int ret;

	ret = 0;

	int **M = G.get_adj_matrix();

	if ((M[u.u][v.u] == GRAY) && (M[u.v][v.v] == GRAY)) {
		ret = 1;
		E.u.u = u.u;
		E.u.v = v.u;
		E.v.u = u.v;
		E.v.v = v.v;
	}
	else if ((M[u.v][v.u] == GRAY) && (M[u.u][v.v] == GRAY)) {
		ret = 1;
		E.u.u = u.v;
		E.u.v = v.u;
		E.v.u = u.u;
		E.v.v = v.v;
	}

	return ret;

} /* get_gray_edges */

/*
 * find_2_cycles
 */
void find_2_cycles(Graph G, Graph &F, Edge E[], 
		   FEdge **BE, FEdge **GE, int &n_edges)
{
	int n, m;
	int u, v;
	int ret;

	n = F.get_num_vertices();
	n_edges = 0;

	for (u = 0; u < n; u++) {
		for (v = u+1; v < n; v++) {
			ret = find_gray_edges(G, E[u], E[v]);
			if (ret != 0) {
				F.add_edge(u, v, EDGE);
				F.add_edge(v, u, EDGE);
				n_edges ++;
			}
		}
	}

	*GE = new FEdge[n_edges];
	*BE = new FEdge[n_edges];

	m = 0;
	for (u = 0; u < n; u++) {
		for (v = u+1; v < n; v++) {
			ret = get_gray_edges(G, E[u], E[v], (*GE)[m]);
			if (ret != 0) {
				(*BE)[m].u = E[u];
				(*BE)[m].v = E[v];
				m ++;
			}
		}
	}

	return;

} /* find_2_cycles */

/* 
 * maximum_cardinality_matching
 */
int *maximum_cardinality_matching(Graph G, Edge label[], FEdge BE[], 
				  int n_fedges)
{
	int i, j;
	int *x = new int[n_fedges];

	int **G_matrix = G.get_adj_matrix();
	int n = G.get_num_vertices();

	my_graph g(n);

	/* Adding the edges in the structure */
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			if (G_matrix[i][j] == 1) {
				add_edge(i,j,g);
			}
		}
	}

	std::vector<graph_traits<my_graph>::vertex_descriptor> mate(n);

	/* find maximum cardinality matching */
	edmonds_maximum_cardinality_matching(g, &mate[0]);

	/* Creating the solution */
	int match_size = (int) matching_size(g, &mate[0]);
	FEdge *res =  new FEdge[match_size];
	i = 0;
	graph_traits<my_graph>::vertex_iterator vi, vi_end;
	for(tie(vi,vi_end) = vertices(g); vi != vi_end; ++vi) {
		if ((mate[*vi] != graph_traits<my_graph>::null_vertex()) 
		    && (*vi < mate[*vi])) {
			res[i].u = label[*vi];
			res[i].v = label[mate[*vi]];
			i++;
		}
	}

	for (i = 0; i < match_size; i++) {
		for (j = 0; j < n_fedges; j++) {
			if (res[i] == BE[i]) {
				x[i] = 1;
				break;
			}
		}
	}

	delete[] res;

	return x;
} /* maximum_cardinality_matching */

/* return_2cycles */
int return_2cycles(int x[], FEdge E[], int n_fedges)
{

	int i, j;

	int cycles = 0;

	bool *is_ladder_edge =  new bool[n_fedges];

	for (i = 0; i < n_fedges; i++) is_ladder_edge[i] = false;

	for (i = 0; i < n_fedges; i++) {
		if (x[i] != 1) continue;

		for (j = i+1; j < n_fedges; j++) {
			if (x[j] != 1) continue;

			/* a edge of ladder graph */
			if (E[i].is_neighbour(E[j])) {
				cycles++;
				is_ladder_edge[i] = true;
				is_ladder_edge[j] = true;
					
			}
		}
		/* independent 2-cycle in ladder graph */
		if (!is_ladder_edge[i]) {
			cycles++;
		}
	}

	delete[] is_ladder_edge;

	return cycles;

} /* return_2cycles */


/* Constructor */
Prob::Prob(int *P, int n, const char *bt)
{
	perm = new int[n];
	perm_size = n;

	for (int i = 0; i < perm_size; i++) {
		perm[i] = P[i];
	}

	btype = new char[strlen(bt)+1];
	strcpy((char *) btype, bt);

	return;
}

/* Destructor */
Prob::~Prob()
{
	delete[] perm;
	delete[] btype;
}

/*
 * calc_breakpoint_trans
 */
void Prob::calc_breakpoint_trans(int P[], int n, int&lb, int &ub)
{
	int i;
	int b;

	b = 0;

	if (P[0] != 1) {
		b = 1;
	}

	for (i = 0; i < n-1; i++) {
		if (P[i+1] != P[i] + 1) {
			b++;
		}
	}

	if (n+1 != P[n-1] + 1) {
		b++;
	}

	lb = b / 3;
	ub = b;

	return;

} /* calc_breakpoint_trans */

/*
 * calc_cycle_graph_trans
 */
void Prob::calc_cycle_graph_trans(int P[], int n, int&lb, int &ub)
{
	int i;
	int c;
	int v;
	Graph G;
	int *Pext = new int[n * 2 + 2];
	int *Ext =  new int[n + 2];

	v = n * 2 + 2;

	Pext[0] = 0;
	for (i = 0; i < n; i++) {
		Pext[i*2 + 1] = -P[i];
		Pext[i*2 + 2] = P[i]; 
	}
	Pext[v-1] = - (n + 1); 

	G.set_num_vertices(v);
	G.set_vertices_array(Pext, v);
	G.create_adj_matrix();

	Ext[0] = 0;
	for (i = 1; i < n+1; i++) {
		Ext[i] = P[i-1];
	}
	Ext[n+1] = n + 1;

	for (i = 1; i <= n+1; i++) {
		G.add_edge(i-1, -i, GRAY);
		G.add_edge(-Ext[i], Ext[i-1], BLACK);
	}

	int **M = G.get_adj_matrix();

	c = count_odd_cycles(M, v);

	G.free_vertices_array();
	G.free_adj_matrix();
	delete Pext;
	delete Ext;

	lb = (n + 1 - c) / 2;
	ub = (3 * (n + 1 - c)) / 4;

	return;

} /* calc_cycle_graph_trans */

/*
 * calc_graph_perm_trans
 */
void Prob::calc_graph_perm_trans(int P[], int n, int&lb, int &ub)
{
	/* TODO 

	int i;
	int c;
	int v;
	Graph G;
	int *Pext = new int[n * 2 + 2];
	int *Ext =  new int[n + 2];

	v = n * 2 + 2;

	Pext[0] = 0;
	for (i = 0; i < n; i++) {
		Pext[i*2 + 1] = -P[i];
		Pext[i*2 + 2] = P[i]; 
	}
	Pext[v-1] = - (n + 1); 

	G.set_num_vertices(v);
	G.set_vertices_array(Pext, v);
	G.create_adj_matrix();

	Ext[0] = 0;
	for (i = 1; i < n+1; i++) {
		Ext[i] = P[i-1];
	}
	Ext[n+1] = n + 1;

	for (i = 1; i <= n+1; i++) {
		G.add_edge(i-1, -i, GRAY);
		G.add_edge(-Ext[i], Ext[i-1], BLACK);
	}

	int **M = G.get_adj_matrix();

	c = count_odd_cycles(M, v);

	G.free_vertices_array();
	G.free_adj_matrix();
	delete Pext;
	delete Ext;

	lb = (n + 1 - c) / 2;
	ub = (3 * (n + 1 - c)) / 4;

	*/
	return;

} /* calc_graph_perm_trans */

/*
 * calc_breakpoint_rev
 */
void Prob::calc_breakpoint_rev(int P[], int n, int&lb, int &ub)
{
	int i;
	int b;

	b = 0;

	if (P[0] != 1) {
		b = 1;
	}

	for (i = 0; i < n-1; i++) {
		if ((P[i+1] != P[i] + 1) && 
		    (P[i+1] != P[i] - 1)) {
			b++;
		}
	}

	if (n+1 != P[n-1] + 1) {
		b++;
	}

	lb = b / 2;
	ub = b;

	return;

} /* calc_breakpoint_rev */

/*
 * calc_cycle_graph_rev
 */
void Prob::calc_cycle_graph_rev(int P[], int n, int&lb, int &ub)
{
	int lb_temp, breakpoints;
	int i, j;
	int c;
	int v;
	int g_black_edges;
	int n_fedges;
	Graph G;
	Graph F;

	int *Ext =  new int[n + 2];
	int *Pi_inv = new int[n + 2];

	v = n + 2;

	/* getting the number of breakpoints of P */
	calc_breakpoint_rev(P, n, lb_temp, breakpoints); //O(n)

	/* Extending the permutation */
	Ext[0] = 0;
	for (i = 1; i < n+1; i++) {
		Ext[i] = P[i-1];
	}
	Ext[n+1] = n + 1;

	/* Creating G(P) */
	G.set_num_vertices(v);
	G.set_vertices_array(v); //O(n)
	G.create_adj_matrix();

	Pi_inv[0] = 0;
	for (i = 1; i < n+1; i++) {
		Pi_inv[Ext[i]] = i;
	}
	Pi_inv[n+1] = n + 1;

	g_black_edges = 0;

	for (i = 1; i < v; i++) {
		if ((Ext[i] != Ext[i-1] + 1) &&
		    (Ext[i] != Ext[i-1] - 1)) {
			G.add_edge(Ext[i], Ext[i-1], BLACK);
			G.add_edge(Ext[i-1], Ext[i], BLACK);
			g_black_edges ++;
		}
		if ((Pi_inv[i] != Pi_inv[i-1] + 1) &&
		    (Pi_inv[i] != Pi_inv[i-1] - 1)) {
			G.add_edge(i-1, i, GRAY);
			G.add_edge(i, i-1, GRAY);
		}
	}

	// /* DEBUG */
	// /* G(pi)*/
	// int **G_matrix = G.get_adj_matrix();
	// int *Glabel = G.get_vertices_array();

	// std::cout << ">>>>";
	// for (i = 0; i < v; i++){
	// 	std::cout << " " << Glabel[i];
	// }
	// std::cout << std::endl;
	// for (i = 0; i < v; i++){
	// 	std::cout << ">> " << Glabel[i];
	// 	for (j = 0; j < v; j++) {
	// 		std::cout << " " << G_matrix[i][j];			
	// 	}
	// 	std::cout << std::endl;
	// }
	// std::cout << std::endl;
	// /* G(pi)*/
	
	/* Creating F(P) */
	int *f_edges = new int[g_black_edges];
	Edge *f_edges_label = new Edge[g_black_edges];
	j = 0;
	for (i = 1; i < v; i++) {
		if ((Ext[i] != Ext[i-1] + 1) &&
		    (Ext[i] != Ext[i-1] - 1)) {
			f_edges_label[j].u = Ext[i];
			f_edges_label[j].v = Ext[i-1];
			f_edges[j] = j;
			j++;
		}
	}

	F.set_num_vertices(g_black_edges);
	F.set_vertices_array(f_edges, g_black_edges); //O(n)
	F.create_adj_matrix();

	FEdge *f_black_edges;
	FEdge *f_gray_edges;

	find_2_cycles(G, F, f_edges_label, &f_black_edges,
		      &f_gray_edges, n_fedges); // O(n^2)

	int *x = maximum_cardinality_matching(F, f_edges_label, 
					      f_black_edges, n_fedges);

	// /* DEBUG */
	// /* F(pi)*/
	// int **F_matrix = F.get_adj_matrix();
	// int *label = F.get_vertices_array();

	// std::cout << ">>>> F Edges: " <<  n_fedges << std::endl;
	// std::cout << ">>>>";
	// for (i = 0; i < g_black_edges; i++){
	// 	std::cout << " " << label[i];
	// }
	// std::cout << std::endl;
	// for (i = 0; i < g_black_edges; i++){
	// 	std::cout << ">> " << label[i];
	// 	for (j = 0; j < g_black_edges; j++) {
	// 		std::cout << " " << F_matrix[i][j];			
	// 	}
	// 	std::cout << std::endl;
	// }
	// /* F(pi)*/
	// /* label */
	// std::cout << ">>>>";
	// for (i = 0; i < g_black_edges; i++){
	// 	std::cout << " " << f_edges_label[i].u << "-"<< f_edges_label[i].v << " |";
	// }
	// std::cout << std::endl;
	// /* gray_edges */
	// std::cout << ">>>> Gray" << std::endl;
	// std::cout << ">>>>";
	// for (i = 0; i < n_fedges; i++){
	// 	std::cout << " (" << f_gray_edges[i].u.u << "," << f_gray_edges[i].u.v 
	// 		  << ") - ("<< f_gray_edges[i].v.u << "," << f_gray_edges[i].v.v
	// 		  << ") |";
	// }
	// std::cout << std::endl;
	// /* black_edges */
	// std::cout << ">>>> Black" << std::endl;
	// std::cout << ">>>>";
	// for (i = 0; i < n_fedges; i++){
	// 	std::cout << " (" << f_black_edges[i].u.u << "," << f_black_edges[i].u.v 
	// 		  << ") - ("<< f_black_edges[i].v.u << "," << f_black_edges[i].v.v
	// 		  << ") |";
	// }
	// std::cout << std::endl;
	// /* X */
	// std::cout << ">>>> X_maximum" << std::endl;
	// std::cout << ">>>>";
	// for (i = 0; i < n_fedges; i++){
	// 	std::cout << " " << x[i] << " ";
	// }
	// std::cout << std::endl;
	// /* ! DEBUG */

	c = return_2cycles(x, f_gray_edges, n_fedges); 

	// /* DEBUG */
	// std::cout << ">>>> C = " << c << std::endl;
	// /* ! DEBUG */

	lb = ((2*breakpoints) - c)/3 ;
	ub = breakpoints - (c/2);

	delete[] x;
	delete[] f_black_edges;
	delete[] f_gray_edges;
	delete[] f_edges_label;
	delete[] f_edges;

	F.free_vertices_array();
	F.free_adj_matrix();

	G.free_vertices_array();
	G.free_adj_matrix();
	delete[] Pi_inv;
	delete[] Ext;

	return;
	
} /* calc_cycle_graph_rev */


/* 
 * get_bound: Returns the model for the model bt.
 */
void Prob::get_bound(int P[], int n, const char *bt, 
		   int &lb, int &ub)
{
	if (strcmp(bt, DEF) == 0) {
		lb = 0;
		ub = n;
	}
	else if (strcmp(bt, TRA_BR) == 0) {
		calc_breakpoint_trans(P, n, lb, ub);
	}
	else if (strcmp(bt, TRA_CG) == 0) {
		calc_cycle_graph_trans(P, n, lb, ub);
	}
	else if (strcmp(bt, TRA_CG) == 0) {
		calc_graph_perm_trans(P, n, lb, ub);
	}
	else if (strcmp(bt, REV_BR) == 0) {
		calc_breakpoint_rev(P, n, lb, ub);
	}
	else if (strcmp(bt, REV_CG) == 0) {
		calc_cycle_graph_rev(P, n, lb, ub);
	}
	else if (strcmp(bt, T_R_BR) == 0) {
		int tlb, tub;
		int rlb, rub;
		calc_breakpoint_trans(P, n, tlb, tub);
		calc_breakpoint_rev(P, n, rlb, rub);

		lb = 0; //((tlb <= rlb) ? tlb : rlb);
		ub = ((tub <= rub) ? tub : rub);
	}
	else if (strcmp(bt, T_R_CB) == 0) {
		int tlb, tub;
		int rlb, rub;
		calc_cycle_graph_trans(P, n, tlb, tub);
		calc_breakpoint_rev(P, n, rlb, rub);

		lb = 0; //((tlb <= rlb) ? tlb : rlb);
		ub = ((tub <= rub) ? tub : rub);
	}
	else if (strcmp(bt, T_R_CC) == 0) {
		int tlb, tub;
		int rlb, rub;
		calc_cycle_graph_trans(P, n, tlb, tub);
		calc_cycle_graph_rev(P, n, rlb, rub);

		lb = 0; //((tlb <= rlb) ? tlb : rlb);
		ub = ((tub <= rub) ? tub : rub);
	}

	std::cout << ">>> LB: " << lb << std::endl;
	std::cout << ">>> UB: " << ub << std::endl;

	return;
} /* get_bound */

/*
 * permutation: Verify if the given array is a permutation.
 */
int Prob::permutation(int P[], int n)
{
	int *num = new int[n];

	memset(num, 0, n*sizeof(int));

	for (int i = 0; i < n; i++) {
		if (num[P[i]] != 0) {
			return FAILURE;
		}
		num[P[i]] ++;
	}

	delete[] num;

	return SUCCESS;
} /* permutation */

/*
 * is_identity: Verify if the given array is the identity 
 * permutation.
 */
int Prob::is_identity(int P[], int n)
{
	for (int i = 0; i < n; i ++) {
		if (P[i] != i+1) {
			return FAILURE;
		}
	}

	return SUCCESS;
} /* is_identity */

/* EOF */
