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
	CP(int *, int, const char *);
	/* Destructor */
	~CP();

	/* trans_dist */
	/* trans_dist_cop: transposition model using cop theory */
	int trans_dist_cop();
	int trans_dist_cop(int P[], int n, const char *bt);
	/* trans_dist_csp: transposition model using csp theory */
	int trans_dist_csp();
	int trans_dist_csp(int P[], int n, const char *bt);

	/* rev_dist */
	/* rev_dist_cop: reversal model using cop theory */
	int rev_dist_cop();
	int rev_dist_cop(int P[], int n, const char *bt);
	/* rev_dist_csp: reversal model using csp theory */
	int rev_dist_csp();
	int rev_dist_csp(int P[], int n, const char *bt);

	/* trans_rev_dist */
	/* trans_rev_dist_csp: trans+rev model using cop theory */
	int trans_rev_dist_cop();
	int trans_rev_dist_cop(int P[], int n, const char *bt);
	/* trans_rev_dist_csp: trans+rev model using csp theory */
	int trans_rev_dist_csp();
	int trans_rev_dist_csp(int P[], int n, const char *bt);

};

#endif
/* ! HAVE_CP_H */
