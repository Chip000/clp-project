/*
 * prob.cpp
 */

#include "../include/prob.h"

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
