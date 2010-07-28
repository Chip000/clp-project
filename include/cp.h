/*
 * cp.h: definitions for integer linear programming functions
 */

#ifndef HAVE_CP_H
#define HAVE_CP_H

#include <ilcp/cp.h>
#include "./prob.h"

class CP: public Prob {
 public:
	/* Constructor */
 CP(int *P, int n, const char *bt):Prob(P, n, bt) {};
	/* Destructor */
	~CP();

	/* trans_dist */
	int trans_dist_cop();
	int trans_dist_cop(int P[], int n, const char *bt);

	/* rev_dist */
	int rev_dist_cop();
	int rev_dist_cop(int P[], int n, const char *bt);

	/* trans_rev_dist */
	int trans_rev_dist_cop();
	int trans_rev_dist_cop(int P[], int n, const char *bt);
};

#endif
/* ! HAVE_CP_H */
