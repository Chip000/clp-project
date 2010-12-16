/*
 * prob.cpp
 */

#include "../include/prob.h"

#define GRAY 1
#define BLACK 2

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
 * calc_cycle_graph_trans
 */
void Prob::calc_cycle_graph_trans(int P[], int n, int&lb, int &ub)
{
	int i, j;
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
	for (i = 1; i <= n+1; i++) {
		Ext[i] = P[i-1];
	}
	Ext[n+1] = n+1;

	for (i = 1; i <= n+1; i++) {
		G.add_edge(i-1, -i, GRAY);
		G.add_edge(-Ext[i], Ext[i-1], BLACK);
	}

	int **M = G.get_adj_matrix();
	int *label = G.get_vertices_array();

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
	else if (strcmp(bt, REV_BR) == 0) {
		calc_breakpoint_rev(P, n, lb, ub);
	}
	else if (strcmp(bt, T_R_BR) == 0) {
		int tlb, tub;
		int rlb, rub;
		calc_breakpoint_trans(P, n, tlb, tub);
		calc_breakpoint_rev(P, n, rlb, rub);

		lb = ((tlb <= rlb) ? tlb : rlb);
		ub = ((tub <= rub) ? tub : rub);
	}
	else if (strcmp(bt, TRA_CG) == 0) {
		calc_cycle_graph_trans(P, n, lb, ub);
	}
	else if (strcmp(bt, T_R_CB) == 0) {
		int tlb, tub;
		int rlb, rub;
		calc_cycle_graph_trans(P, n, tlb, tub);
		calc_breakpoint_rev(P, n, rlb, rub);

		lb = ((tlb <= rlb) ? tlb : rlb);
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
