/*
 * prob.h: definitions for the problem functions
 */

#ifndef HAVE_PROB_H
#define HAVE_PROB_H

#include <iostream>
#include <cstring>
#include "../include/graph.h"
#include "../include/edge.h"
#include "../include/f_edge.h"

#define DEBUG 1

#define SUCCESS 0
#define FAILURE -1

/* bounds */
#define DEF "def"
#define REV_BR "rev_br"
#define REV_CG "rev_cg"

#define TRA_BR "tra_br"
#define TRA_CG "tra_cg"

#define T_R_BR "t_r_br"
#define T_R_CB "t_r_cb"
#define T_R_CC "t_r_cc"

class Prob {
 protected:
	int *perm;
	int perm_size;
	const char *btype;
 public:
	/* Constructor */
	Prob(int *P, int n, const char *bt);
	/* Destructor */
	~Prob();

	/* calc_breakpoint_trans */
	void calc_breakpoint_trans(int P[], int n, int&lb, int &ub);
	/* calc_cycle_graph_trans */
	void calc_cycle_graph_trans(int P[], int n, int&lb, int &ub);
	/* calc_graph_perm_trans */
	void calc_graph_perm_trans(int P[], int n, int&lb, int &ub);

	/* calc_breakpoint_rev */
	void calc_breakpoint_rev(int P[], int n, int&lb, int &ub);
	/* calc_cycle_graph_rev */
	void calc_cycle_graph_rev(int P[], int n, int&lb, int &ub);

	/* get_bound: Returns the model for the model bt. */
	void get_bound(int P[], int n, const char *bt, 
			   int &lb, int &ub);
	/* permutation: Verify if the given array is a permutation. */
	int permutation(int P[], int n);
	/* is_identity: Verify if the given array is the identity 
	   permutation. */
	int is_identity(int P[], int n);
};

#endif

/* ! HAVE_PROB_H */
