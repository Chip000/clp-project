/*
 * cp.cpp
 */

#include "../include/cp.h"
#include <cstring>

/* Constructor */
CP::CP(int *P, int n, const char *bt):
	Prob(P, n, bt) {}

/* Destructor */
CP::~CP()
{
	/* Using the Destructor from Prob */
}

/* trans_dist */
/*
 * trans_dist_cop: transposition model using cop theory
 */
int CP::trans_dist_cop()
{
	return CP::trans_dist_cop(this->perm, this->perm_size, 
				  this->btype);
}

int CP::trans_dist_cop(int P[], int n, const char *bt)
{
	IloEnv env;
	int dist;
	int lb, ub;
	IloIntArray Pi(env, n+1);
	IloIntArray Sigma(env, n+1);

	int i, j, k, l;
	int count;

	for (count = 0; count < n; count ++) {
		Pi[count+1] = P[count];
		Sigma[count+1] = count + 1;
	}

	if (is_identity(P, n) == SUCCESS) {
		return 0;
	}

	dist = FAILURE;

	try {
		get_bound(P, n, bt, lb, ub);

		/* var B_{ijk} */
		IloArray<IloArray<IloBoolVarArray> > B(env, n+2);
		for (i = 0; i < n+2; i++) {
			B[i] = IloArray<IloBoolVarArray> (env, n+1);
			for (j = 0; j < n+1; j++) {
				B[i][j] = IloBoolVarArray (env, n);
			}
		}

		/* var T_{abck} */
		IloArray<IloArray<IloArray<IloBoolVarArray> > > T(env, n+2);
		for (i = 0; i < n+2; i++) {
			T[i] = IloArray<IloArray<IloBoolVarArray> > (env, n+2);
			for (j = 0; j < n+2; j++) {
				T[i][j] = IloArray<IloBoolVarArray> (env, n+2);
				for (l = 0; l < n+2; l++) {
					T[i][j][l] = IloBoolVarArray (env, n);
				}
			}
		}

		/* var TD_{k} */
		IloBoolVarArray TD(env, n);

		/* Constraints */ 
		IloModel model(env);

		/* Common Constraints*/
		/* Initial and final permutation are correct */
		for (i = 1; i < n+1; i++) {
			model.add(B[i][Pi[i]][0] == 1);
			model.add(B[i][Sigma[i]][n-1] == 1);
		}

		/* any position of permutation has exactly one 
		   value associated to it */
		for (i = 1; i < n+1; i++) {
			for (k = 0; k < n; k++) {
				IloExpr sum(env);

				for (j = 1; j < n+1; j++) {
					sum += B[i][j][k];
				}

				model.add(sum == 1);
				/* destroy the sum object */
				sum.end();
			}
		}

		/* every value is assigned to one position of 
		   each permutation */
		for (j = 1; j < n+1; j++) {
			for (k = 0; k < n; k++) {
				IloExpr sum(env);

				for (i = 1; i < n+1; i++) {
					sum += B[i][j][k];
				}

				model.add(sum == 1);
				/* destroy the sum object */
				sum.end();
			}
		}
		/* End Common Constraints */

		/* Transpositions Constraints */
		/* if kth permutation does not alter the permutation,
		   none of subsequent will do so */
		model.add(TD[0] == 1);
		for (k = 1; k < n; k++) {
			model.add(TD[k] <= TD[k-1]);
		}

		/* at most one transposition is done at each step */
		for (k = 0; k < n; k++) {
			IloExpr sum(env);

			for (int a = 1; a < n; a++) {
				for (int b = a+1; b < n+1; b++) {
					for (int c = b+1; c < n+2; c++) {
						sum += T[a][b][c][k];
					}
				}
			}

			model.add(sum == TD[k]);
			/* destroy the sum object */
			sum.end();
		}

		/*  changes in permutation by transposition */
		/* i < a or i > c */
		for (i = 1; i < n+1; i++) {
			for (j = 1; j < n+1; j++) {
				for (k = 1; k < n; k++) {

					IloExpr sum(env);
			    
					for (int a = i+1; a < n; a++) {
						for (int b = a+1; b < n+1; b++) {
							for (int c = b+1; c < n+2; c++) {
								sum += T[a][b][c][k];
							}
						}
					}
					
					for (int a = 1; a < n; a++) {
						for (int b = a+1; b < n+1; b++) {
							for (int c = b+1; c < i+1; c++) {
								sum += T[a][b][c][k];
							}
						}
					}

					sum += B[i][j][k-1] + (1 - TD[k]) - B[i][j][k];

					model.add(sum <= 1);
					/* destroy the sum object */
					sum.end();

				}
			}
		}

		/* a <= i < a + c - b */
		for (int a = 1; a < n+2; a++) {
			for (int b = 1; b < n+2; b++) {
				for (int c = 1; c < n+2; c++) {
					for (i = a; i <= a+c-b-1; i++) {
						for (j = 1; j < n+1; j++) {
							for (k = 1; k < n; k++) {
								if ((a < b) && (b < c)) {
									model.add(T[a][b][c][k] + B[b-a+i][j][k-1] - B[i][j][k] <= 1);
								}
							}
						}
					}
				}
			}
		}

		/* a + c - b <= i <= b */
		for (int a = 1; a < n+2; a++) {
			for (int b = 1; b < n+2; b++) {
				for (int c = 1; c < n+2; c++) {
					for (i = a+c-b; i <= c-1; i++) {
						for (j = 1; j < n+1; j++) {
							for (k = 1; k < n; k++) {
								if ((a < b) && (b < c)) {
									model.add(T[a][b][c][k] + B[b-c+i][j][k-1] - B[i][j][k] <= 1);
								}
							}
						}
					}
				}
			}
		}
		/* End Transposition Constraints */

		/* Bounds */
		for (k = 1; k < n; k++) {
			model.add(TD[k] * k <= ub);
			model.add(TD[k] * n + k - 1 >= lb);
		}

		/* End Constraints */

		/* objective function */
		IloExpr obj(env);

		for (k = 1; k < n; k++) {
			obj += TD[k];
		}

		model.add(IloMinimize(env, obj));

		/* Solving the problem */
		IloCP cp(env);
		cp.extract(model);
		if (cp.solve()) {
			cp.out() << std::endl ;
			cp.out() << "Optimal value: ";
			dist = cp.getValue(obj);
			cp.out() <<  dist << std::endl;
		}

		obj.end();
	}
	catch (IloException &ex) {
		env.out() << "Error: " << ex.getMessage() << std::endl;
		return FAILURE;
	}

	env.end();

	return dist;

}
/* trans_dist_cop */

/*
 * trans_dist_csp: transposition model using csp theory
 */
int CP::trans_dist_csp()
{
	return CP::trans_dist_csp(this->perm, this->perm_size, 
				  this->btype);
}

int CP::trans_dist_csp(int P[], int n, const char *bt)
{
	IloEnv env;
	int dist;
	int lb, ub;
	IloIntArray Pi(env, n+1);
	IloIntArray Sigma(env, n+1);

	int i, j, k, l;
	int count;

	for (count = 0; count < n; count ++) {
		Pi[count+1] = P[count];
		Sigma[count+1] = count + 1;
	}

	if (is_identity(P, n) == SUCCESS) {
		return 0;
	}

	dist = FAILURE;

	try {
		get_bound(P, n, bt, lb, ub);

		/* var B_{ijk} */
		IloArray<IloArray<IloBoolVarArray> > B(env, n+2);
		for (i = 0; i < n+2; i++) {
			B[i] = IloArray<IloBoolVarArray> (env, n+1);
			for (j = 0; j < n+1; j++) {
				B[i][j] = IloBoolVarArray (env, n);
			}
		}

		/* var T_{abck} */
		IloArray<IloArray<IloArray<IloBoolVarArray> > > T(env, n+2);
		for (i = 0; i < n+2; i++) {
			T[i] = IloArray<IloArray<IloBoolVarArray> > (env, n+2);
			for (j = 0; j < n+2; j++) {
				T[i][j] = IloArray<IloBoolVarArray> (env, n+2);
				for (l = 0; l < n+2; l++) {
					T[i][j][l] = IloBoolVarArray (env, n);
				}
			}
		}

		/* var TD_{k} */
		IloBoolVarArray TD(env, n);

		/* Constraints */ 
		IloModel model(env);

		/* Common Constraints*/
		/* Initial and final permutation are correct */
		for (i = 1; i < n+1; i++) {
			model.add(B[i][Pi[i]][0] == 1);
			model.add(B[i][Sigma[i]][n-1] == 1);
		}

		/* any position of permutation has exactly one 
		   value associated to it */
		for (i = 1; i < n+1; i++) {
			for (k = 0; k < n; k++) {
				IloExpr sum(env);

				for (j = 1; j < n+1; j++) {
					sum += B[i][j][k];
				}

				model.add(sum == 1);
				/* destroy the sum object */
				sum.end();
			}
		}

		/* every value is assigned to one position of 
		   each permutation */
		for (j = 1; j < n+1; j++) {
			for (k = 0; k < n; k++) {
				IloExpr sum(env);

				for (i = 1; i < n+1; i++) {
					sum += B[i][j][k];
				}

				model.add(sum == 1);
				/* destroy the sum object */
				sum.end();
			}
		}
		/* End Common Constraints */

		/* Transpositions Constraints */
		/* if kth permutation does not alter the permutation,
		   none of subsequent will do so */
		model.add(TD[0] == 1);
		for (k = 1; k < n; k++) {
			model.add(TD[k] <= TD[k-1]);
		}

		/* at most one transposition is done at each step */
		for (k = 0; k < n; k++) {
			IloExpr sum(env);

			for (int a = 1; a < n; a++) {
				for (int b = a+1; b < n+1; b++) {
					for (int c = b+1; c < n+2; c++) {
						sum += T[a][b][c][k];
					}
				}
			}

			model.add(sum == TD[k]);
			/* destroy the sum object */
			sum.end();
		}

		/*  changes in permutation by transposition */
		/* i < a or i > c */
		for (i = 1; i < n+1; i++) {
			for (j = 1; j < n+1; j++) {
				for (k = 1; k < n; k++) {

					IloExpr sum(env);
			    
					for (int a = i+1; a < n; a++) {
						for (int b = a+1; b < n+1; b++) {
							for (int c = b+1; c < n+2; c++) {
								sum += T[a][b][c][k];
							}
						}
					}
					
					for (int a = 1; a < n; a++) {
						for (int b = a+1; b < n+1; b++) {
							for (int c = b+1; c < i+1; c++) {
								sum += T[a][b][c][k];
							}
						}
					}

					sum += B[i][j][k-1] + (1 - TD[k]) - B[i][j][k];

					model.add(sum <= 1);
					/* destroy the sum object */
					sum.end();

				}
			}
		}

		/* a <= i < a + c - b */
		for (int a = 1; a < n+2; a++) {
			for (int b = 1; b < n+2; b++) {
				for (int c = 1; c < n+2; c++) {
					for (i = a; i <= a+c-b-1; i++) {
						for (j = 1; j < n+1; j++) {
							for (k = 1; k < n; k++) {
								if ((a < b) && (b < c)) {
									model.add(T[a][b][c][k] + B[b-a+i][j][k-1] - B[i][j][k] <= 1);
								}
							}
						}
					}
				}
			}
		}

		/* a + c - b <= i <= b */
		for (int a = 1; a < n+2; a++) {
			for (int b = 1; b < n+2; b++) {
				for (int c = 1; c < n+2; c++) {
					for (i = a+c-b; i <= c-1; i++) {
						for (j = 1; j < n+1; j++) {
							for (k = 1; k < n; k++) {
								if ((a < b) && (b < c)) {
									model.add(T[a][b][c][k] + B[b-c+i][j][k-1] - B[i][j][k] <= 1);
								}
							}
						}
					}
				}
			}
		}
		/* End Transposition Constraints */

		/* Bounds */
		IloExpr obj(env);

		for (k = 1; k < n; k++) {
			obj += TD[k];
		}

		/* lb <= obj <= ub */
		model.add(obj <= ub);
		model.add(obj >= lb);

		/* End Constraints */

		/* Solving the problem - Bottom-Up */
		for (int lim = lb; lim <= ub; lim++) {
			IloModel m(env);

			m.add(model);
			m.add(obj == lim);

			IloCP cp(env);
			cp.extract(m);

			if (cp.solve()) {
				cp.out() << std::endl ;
				cp.out() << "Optimal value: ";
				dist = cp.getValue(obj);
				cp.out() <<  dist << std::endl;
				break;
			}

		}
		obj.end();

	}
	catch (IloException &ex) {
		env.out() << "Error: " << ex.getMessage() << std::endl;
		return FAILURE;
	}

	env.end();

	return dist;

}
/* trans_dist_csp */
/* trans_dist */

/* rev_dist */
/*
 * rev_dist_cop: transposition model using cop theory
 */
int CP::rev_dist_cop()
{
	return CP::rev_dist_cop(this->perm, this->perm_size, 
				this->btype);
}

int CP::rev_dist_cop(int P[], int n, const char *bt)
{
	IloEnv env;
	int dist;
	int lb, ub;
	IloIntArray Pi(env, n+1);
	IloIntArray Sigma(env, n+1);

	int i, j, k;
	int count;

	for (count = 0; count < n; count ++) {
		Pi[count+1] = P[count];
		Sigma[count+1] = count + 1;
	}

	if (is_identity(P, n) == SUCCESS) {
		return 0;
	}

	dist = FAILURE;

	try {
		get_bound(P, n, bt, lb, ub);

		/* var B_{ijk} */
		IloArray<IloArray<IloBoolVarArray> > B(env, n+1);
		for (i = 0; i < n+1; i++) {
			B[i] = IloArray<IloBoolVarArray> (env, n+1);
			for (int j = 0; j < n+1; j++) {
				B[i][j] = IloBoolVarArray (env, n);
			}
		}

		/* var R_{abk} */
		IloArray<IloArray<IloBoolVarArray> > R(env, n+1);
		for (i = 0; i < n+1; i++) {
			R[i] = IloArray<IloBoolVarArray> (env, n+1);
			for (int j = 0; j < n+1; j++) {
				R[i][j] = IloBoolVarArray (env, n);
			}
		}

		/* var RD_{k} */
		IloBoolVarArray RD(env, n);

		/* Constraints */ 
		IloModel model(env);

		/* Common Constraints*/
		/* Initial and final permutation are correct */
		for (i = 1; i < n+1; i++) {
			model.add(B[i][Pi[i]][0] == 1);
			model.add(B[i][Sigma[i]][n-1] == 1);
		}

		/* any position of permutation has exactly one 
		   value associated to it */
		for (i = 1; i < n+1; i++) {
			for (k = 0; k < n; k++) {
				IloExpr sum(env);

				for (j = 1; j < n+1; j++) {
					sum += B[i][j][k];
				}

				model.add(sum == 1);
				/* destroy the sum object */
				sum.end();
			}
		}

		/* every value is assigned to one position of 
		   each permutation */
		for (j = 1; j < n+1; j++) {
			for (k = 0; k < n; k++) {
				IloExpr sum(env);

				for (i = 1; i < n+1; i++) {
					sum += B[i][j][k];
				}

				model.add(sum == 1);
				/* destroy the sum object */
				sum.end();
			}
		}
		/* End Common Constraints */

		/* Reversal Constraints */
		/* if kth permutation does not alter the permutation,
		   none of subsequent will do so */
		model.add(RD[0] == 1);
		for (k = 1; k < n; k++) {
			model.add(RD[k] <= RD[k-1]);
		}

		/* at most one reversal is done at each step */
		for (k = 0; k < n; k++) {
			IloExpr sum(env);

			for (int a = 1; a < n; a++) {
				for (int b = a+1; b < n+1; b++) {
					sum += R[a][b][k];
				}
			}

			model.add(sum == RD[k]);
			/* destroy the sum object */
			sum.end();
		}

		/*  changes in permutation by reversal */
		/* i < a or i > b */
		for (i = 1; i < n+1; i++) {
			for (j = 1; j < n+1; j++) {
				for (k = 1; k < n; k++) {

					IloExpr sum(env);
			    
					for (int a = i+1; a < n; a++) {
						for (int b = a+1; b < n+1; b++) {
							sum += R[a][b][k];
						}
					}
					
					for (int a = 1; a < n; a++) {
						for (int b = a+1; b < i; b++) {
							sum += R[a][b][k];
						}
					}

					sum += B[i][j][k-1] + (1 - RD[k]) - B[i][j][k];

					model.add(sum <= 1);
					/* destroy the sum object */
					sum.end();

				}
			}
		}

		/* a <= i <= b */
		for (int a = 1; a < n; a++) {
			for (int b = a+1; b < n+1; b++) {
				for (i = a; i <= b; i++) {
					for (j = 1; j < n+1; j++) {
						for (k = 1; k < n; k++) {
							model.add(R[a][b][k] + B[b+a-i][j][k-1] - B[i][j][k] <= 1);
						}
					}
				}
			}
		}
		/* End Reversal Constraints */

		/* Bounds */
		IloExpr obj(env);

		for (k = 1; k < n; k++) {
			obj += RD[k];
		}

		model.add(obj >= lb);
		model.add(obj <= ub);

		/* End Constraints */

		/* objective function */

		model.add(IloMinimize(env, obj));

		/* Solving the problem */
		IloCP cp(env);
		cp.extract(model);
		if (cp.solve()) {
			cp.out() << std::endl ;
			cp.out() << "Optimal value: ";
			dist = cp.getValue(obj);
			cp.out() <<  dist << std::endl;
		}

		obj.end();
	}
	catch (IloException &ex) {
		env.out() << "Error: " << ex.getMessage() << std::endl;
		return FAILURE;
	}

	env.end();

	return dist;

}
/* rev_dist_cop */
/*
 * rev_dist_csp: transposition model using csp theory
 */
int CP::rev_dist_csp()
{
	return CP::rev_dist_csp(this->perm, this->perm_size, 
				this->btype);
}

int CP::rev_dist_csp(int P[], int n, const char *bt)
{
	IloEnv env;
	int dist;
	int lb, ub;
	IloIntArray Pi(env, n+1);
	IloIntArray Sigma(env, n+1);

	int i, j, k;
	int count;

	for (count = 0; count < n; count ++) {
		Pi[count+1] = P[count];
		Sigma[count+1] = count + 1;
	}

	if (is_identity(P, n) == SUCCESS) {
		return 0;
	}

	dist = FAILURE;

	try {
		get_bound(P, n, bt, lb, ub);

		/* var B_{ijk} */
		IloArray<IloArray<IloBoolVarArray> > B(env, n+1);
		for (i = 0; i < n+1; i++) {
			B[i] = IloArray<IloBoolVarArray> (env, n+1);
			for (int j = 0; j < n+1; j++) {
				B[i][j] = IloBoolVarArray (env, n);
			}
		}

		/* var R_{abk} */
		IloArray<IloArray<IloBoolVarArray> > R(env, n+1);
		for (i = 0; i < n+1; i++) {
			R[i] = IloArray<IloBoolVarArray> (env, n+1);
			for (int j = 0; j < n+1; j++) {
				R[i][j] = IloBoolVarArray (env, n);
			}
		}

		/* var RD_{k} */
		IloBoolVarArray RD(env, n);

		/* Constraints */ 
		IloModel model(env);

		/* Common Constraints*/
		/* Initial and final permutation are correct */
		for (i = 1; i < n+1; i++) {
			model.add(B[i][Pi[i]][0] == 1);
			model.add(B[i][Sigma[i]][n-1] == 1);
		}

		/* any position of permutation has exactly one 
		   value associated to it */
		for (i = 1; i < n+1; i++) {
			for (k = 0; k < n; k++) {
				IloExpr sum(env);

				for (j = 1; j < n+1; j++) {
					sum += B[i][j][k];
				}

				model.add(sum == 1);
				/* destroy the sum object */
				sum.end();
			}
		}

		/* every value is assigned to one position of 
		   each permutation */
		for (j = 1; j < n+1; j++) {
			for (k = 0; k < n; k++) {
				IloExpr sum(env);

				for (i = 1; i < n+1; i++) {
					sum += B[i][j][k];
				}

				model.add(sum == 1);
				/* destroy the sum object */
				sum.end();
			}
		}
		/* End Common Constraints */

		/* Reversal Constraints */
		/* if kth permutation does not alter the permutation,
		   none of subsequent will do so */
		model.add(RD[0] == 1);
		for (k = 1; k < n; k++) {
			model.add(RD[k] <= RD[k-1]);
		}

		/* at most one reversal is done at each step */
		for (k = 0; k < n; k++) {
			IloExpr sum(env);

			for (int a = 1; a < n; a++) {
				for (int b = a+1; b < n+1; b++) {
					sum += R[a][b][k];
				}
			}

			model.add(sum == RD[k]);
			/* destroy the sum object */
			sum.end();
		}

		/*  changes in permutation by reversal */
		/* i < a or i > b */
		for (i = 1; i < n+1; i++) {
			for (j = 1; j < n+1; j++) {
				for (k = 1; k < n; k++) {

					IloExpr sum(env);
			    
					for (int a = i+1; a < n; a++) {
						for (int b = a+1; b < n+1; b++) {
							sum += R[a][b][k];
						}
					}
					
					for (int a = 1; a < n; a++) {
						for (int b = a+1; b < i; b++) {
							sum += R[a][b][k];
						}
					}

					sum += B[i][j][k-1] + (1 - RD[k]) - B[i][j][k];

					model.add(sum <= 1);
					/* destroy the sum object */
					sum.end();

				}
			}
		}

		/* a <= i <= b */
		for (int a = 1; a < n; a++) {
			for (int b = a+1; b < n+1; b++) {
				for (i = a; i <= b; i++) {
					for (j = 1; j < n+1; j++) {
						for (k = 1; k < n; k++) {
							model.add(R[a][b][k] + B[b+a-i][j][k-1] - B[i][j][k] <= 1);
						}
					}
				}
			}
		}
		/* End Reversal Constraints */

		/* Bounds */
		IloExpr obj(env);

		for (k = 1; k < n; k++) {
			obj += RD[k];
		}

		model.add(obj <= ub);
		model.add(obj >= lb);

		/* End Constraints */

		/* Solving the problem - Bottom-Up */
		for (int lim = lb; lim <= ub; lim++) {
			IloModel m(env);

			m.add(model);
			m.add(obj == lim);

			IloCP cp(env);
			cp.extract(m);

			if (cp.solve()) {
				cp.out() << std::endl ;
				cp.out() << "Optimal value: ";
				dist = cp.getValue(obj);
				cp.out() <<  dist << std::endl;
				break;
			}

		}

		obj.end();
	}
	catch (IloException &ex) {
		env.out() << "Error: " << ex.getMessage() << std::endl;
		return FAILURE;
	}

	env.end();

	return dist;

}
/* rev_dist_csp */
/* rev_dist */

/* trans_rev_dist */
/*
 * trans_rev_dist_cop: Trans_rev using cop theory
 */
int CP::trans_rev_dist_cop()
{
	return CP::trans_rev_dist_cop(this->perm, this->perm_size, 
				      this->btype);
}

int CP::trans_rev_dist_cop(int P[], int n, const char *bt)
{
	IloEnv env;
	int dist;
	int lb, ub;
	IloIntArray Pi(env, n+1);
	IloIntArray Sigma(env, n+1);

	int i, j, k, l;
	int count;

	for (count = 0; count < n; count ++) {
		Pi[count+1] = P[count];
		Sigma[count+1] = count + 1;
	}

	if (is_identity(P, n) == SUCCESS) {
		return 0;
	}

	dist = FAILURE;

	try {
		get_bound(P, n, bt, lb, ub);

		/* var B_{ijk} */
		IloArray<IloArray<IloBoolVarArray> > B(env, n+1);
		for (i = 0; i < n+1; i++) {
			B[i] = IloArray<IloBoolVarArray> (env, n+1);
			for (int j = 0; j < n+1; j++) {
				B[i][j] = IloBoolVarArray (env, n);
			}
		}

		/* var R_{abk} */
		IloArray<IloArray<IloBoolVarArray> > R(env, n+1);
		for (i = 0; i < n+1; i++) {
			R[i] = IloArray<IloBoolVarArray> (env, n+1);
			for (int j = 0; j < n+1; j++) {
				R[i][j] = IloBoolVarArray (env, n);
			}
		}

		/* var RD_{k} */
		IloBoolVarArray RD(env, n);

		/* var T_{abck} */
		IloArray<IloArray<IloArray<IloBoolVarArray> > > T(env, n+2);
		for (i = 0; i < n+2; i++) {
			T[i] = IloArray<IloArray<IloBoolVarArray> > (env, n+2);
			for (j = 0; j < n+2; j++) {
				T[i][j] = IloArray<IloBoolVarArray> (env, n+2);
				for (l = 0; l < n+2; l++) {
					T[i][j][l] = IloBoolVarArray (env, n);
				}
			}
		}

		/* var TD_{k} */
		IloBoolVarArray TD(env, n);

		/* var ZD_{k} */
		IloBoolVarArray ZD(env, n);

		/* Constraints */ 
		IloModel model(env);

		/* Common Constraints*/
		/* Initial and final permutation are correct */
		for (i = 1; i < n+1; i++) {
			model.add(B[i][Pi[i]][0] == 1);
			model.add(B[i][Sigma[i]][n-1] == 1);
		}

		/* any position of permutation has exactly one 
		   value associated to it */
		for (i = 1; i < n+1; i++) {
			for (k = 0; k < n; k++) {
				IloExpr sum(env);

				for (j = 1; j < n+1; j++) {
					sum += B[i][j][k];
				}

				model.add(sum == 1);
				/* destroy the sum object */
				sum.end();
			}
		}

		/* every value is assigned to one position of 
		   each permutation */
		for (j = 1; j < n+1; j++) {
			for (k = 0; k < n; k++) {
				IloExpr sum(env);

				for (i = 1; i < n+1; i++) {
					sum += B[i][j][k];
				}

				model.add(sum == 1);
				/* destroy the sum object */
				sum.end();
			}
		}
		/* End Common Constraints */

		/* Transpositions Constraints */
		/* at most one transposition is done at each step */
		for (k = 0; k < n; k++) {
			IloExpr sum(env);

			for (int a = 1; a < n; a++) {
				for (int b = a+1; b < n+1; b++) {
					for (int c = b+1; c < n+2; c++) {
						sum += T[a][b][c][k];
					}
				}
			}

			model.add(sum == TD[k]);
			/* destroy the sum object */
			sum.end();
		}

		/*  changes in permutation by transposition */
		/* i < a or i > c */
		for (i = 1; i < n+1; i++) {
			for (j = 1; j < n+1; j++) {
				for (k = 1; k < n; k++) {

					IloExpr sum(env);
			    
					for (int a = i+1; a < n; a++) {
						for (int b = a+1; b < n+1; b++) {
							for (int c = b+1; c < n+2; c++) {
								sum += T[a][b][c][k];
							}
						}
					}
					
					for (int a = 1; a < n; a++) {
						for (int b = a+1; b < n+1; b++) {
							for (int c = b+1; c < i+1; c++) {
								sum += T[a][b][c][k];
							}
						}
					}

					sum += B[i][j][k-1] + (1 - ZD[k]) - B[i][j][k];

					model.add(sum <= 1);
					/* destroy the sum object */
					sum.end();

				}
			}
		}

		/* a <= i < a + c - b */
		for (int a = 1; a < n+2; a++) {
			for (int b = 1; b < n+2; b++) {
				for (int c = 1; c < n+2; c++) {
					for (i = a; i <= a+c-b-1; i++) {
						for (j = 1; j < n+1; j++) {
							for (k = 1; k < n; k++) {
								if ((a < b) && (b < c)) {
									model.add(T[a][b][c][k] + B[b-a+i][j][k-1] - B[i][j][k] <= 1);
								}
							}
						}
					}
				}
			}
		}

		/* a + c - b <= i <= b */
		for (int a = 1; a < n+2; a++) {
			for (int b = 1; b < n+2; b++) {
				for (int c = 1; c < n+2; c++) {
					for (i = a+c-b; i <= c-1; i++) {
						for (j = 1; j < n+1; j++) {
							for (k = 1; k < n; k++) {
								if ((a < b) && (b < c)) {
									model.add(T[a][b][c][k] + B[b-c+i][j][k-1] - B[i][j][k] <= 1);
								}
							}
						}
					}
				}
			}
		}
		/* End Transposition Constraints */

		/* Reversal Constraints */
		/* at most one reversal is done at each step */
		for (k = 0; k < n; k++) {
			IloExpr sum(env);

			for (int a = 1; a < n; a++) {
				for (int b = a+1; b < n+1; b++) {
					sum += R[a][b][k];
				}
			}

			model.add(sum == RD[k]);
			/* destroy the sum object */
			sum.end();
		}

		/*  changes in permutation by reversal */
		/* i < a or i > b */
		for (i = 1; i < n+1; i++) {
			for (j = 1; j < n+1; j++) {
				for (k = 1; k < n; k++) {

					IloExpr sum(env);
			    
					for (int a = i+1; a < n; a++) {
						for (int b = a+1; b < n+1; b++) {
							sum += R[a][b][k];
						}
					}
					
					for (int a = 1; a < n; a++) {
						for (int b = a+1; b < i; b++) {
							sum += R[a][b][k];
						}
					}

					sum += B[i][j][k-1] + (1 - ZD[k]) - B[i][j][k];

					model.add(sum <= 1);
					/* destroy the sum object */
					sum.end();

				}
			}
		}

		/* a <= i <= b */
		for (int a = 1; a < n; a++) {
			for (int b = a+1; b < n+1; b++) {
				for (i = a; i <= b; i++) {
					for (j = 1; j < n+1; j++) {
						for (k = 1; k < n; k++) {
							model.add(R[a][b][k] + B[b+a-i][j][k-1] - B[i][j][k] <= 1);
						}
					}
				}
			}
		}
		/* End Reversal Constraints */

		/* Transposition+Reversal Constraints */
		/* if kth permutation does not alter the permutation,
		   none of subsequent will do so */
		model.add(ZD[0] == 1);
		for (k = 1; k < n; k++) {
			model.add(ZD[k] <= ZD[k-1]);
		}

		/* At most one operation (a reversal or a transposition 
		   is done */
		for (k = 1; k < n; k++) {
			model.add(TD[k] + RD[k] == ZD[k]);
		}

		/* End Transposition+Reversal Constraints */

		/* Bounds */

		/* objective function */
		IloExpr obj(env);

		for (k = 1; k < n; k++) {
			obj += ZD[k];
		}
		model.add(obj <= ub);
		model.add(obj >= lb);

		model.add(IloMinimize(env, obj));

		/* Solving the problem */
		IloCP cp(env);
		cp.extract(model);
		if (cp.solve()) {
			cp.out() << std::endl ;
			cp.out() << "Optimal value: ";
			dist = cp.getValue(obj);
			cp.out() <<  dist << std::endl;
		}

		obj.end();
	}
	catch (IloException &ex) {
		env.out() << "Error: " << ex.getMessage() << std::endl;
		return FAILURE;
	}

	env.end();

	return dist;

}
/* trans_rev_dist_cop */

/*
 * trans_rev_dist_csp: Trans_rev using csp theory
 */
int CP::trans_rev_dist_csp()
{
	return CP::trans_rev_dist_csp(this->perm, this->perm_size, 
				      this->btype);
}

int CP::trans_rev_dist_csp(int P[], int n, const char *bt)
{
	IloEnv env;
	int dist;
	int lb, ub;
	IloIntArray Pi(env, n+1);
	IloIntArray Sigma(env, n+1);

	int i, j, k, l;
	int count;

	for (count = 0; count < n; count ++) {
		Pi[count+1] = P[count];
		Sigma[count+1] = count + 1;
	}

	if (is_identity(P, n) == SUCCESS) {
		return 0;
	}

	dist = FAILURE;

	try {
		get_bound(P, n, bt, lb, ub);

		/* var B_{ijk} */
		IloArray<IloArray<IloBoolVarArray> > B(env, n+1);
		for (i = 0; i < n+1; i++) {
			B[i] = IloArray<IloBoolVarArray> (env, n+1);
			for (int j = 0; j < n+1; j++) {
				B[i][j] = IloBoolVarArray (env, n);
			}
		}

		/* var R_{abk} */
		IloArray<IloArray<IloBoolVarArray> > R(env, n+1);
		for (i = 0; i < n+1; i++) {
			R[i] = IloArray<IloBoolVarArray> (env, n+1);
			for (int j = 0; j < n+1; j++) {
				R[i][j] = IloBoolVarArray (env, n);
			}
		}

		/* var RD_{k} */
		IloBoolVarArray RD(env, n);

		/* var T_{abck} */
		IloArray<IloArray<IloArray<IloBoolVarArray> > > T(env, n+2);
		for (i = 0; i < n+2; i++) {
			T[i] = IloArray<IloArray<IloBoolVarArray> > (env, n+2);
			for (j = 0; j < n+2; j++) {
				T[i][j] = IloArray<IloBoolVarArray> (env, n+2);
				for (l = 0; l < n+2; l++) {
					T[i][j][l] = IloBoolVarArray (env, n);
				}
			}
		}

		/* var TD_{k} */
		IloBoolVarArray TD(env, n);

		/* var ZD_{k} */
		IloBoolVarArray ZD(env, n);

		/* Constraints */ 
		IloModel model(env);

		/* Common Constraints*/
		/* Initial and final permutation are correct */
		for (i = 1; i < n+1; i++) {
			model.add(B[i][Pi[i]][0] == 1);
			model.add(B[i][Sigma[i]][n-1] == 1);
		}

		/* any position of permutation has exactly one 
		   value associated to it */
		for (i = 1; i < n+1; i++) {
			for (k = 0; k < n; k++) {
				IloExpr sum(env);

				for (j = 1; j < n+1; j++) {
					sum += B[i][j][k];
				}

				model.add(sum == 1);
				/* destroy the sum object */
				sum.end();
			}
		}

		/* every value is assigned to one position of 
		   each permutation */
		for (j = 1; j < n+1; j++) {
			for (k = 0; k < n; k++) {
				IloExpr sum(env);

				for (i = 1; i < n+1; i++) {
					sum += B[i][j][k];
				}

				model.add(sum == 1);
				/* destroy the sum object */
				sum.end();
			}
		}
		/* End Common Constraints */

		/* Transpositions Constraints */
		/* at most one transposition is done at each step */
		for (k = 0; k < n; k++) {
			IloExpr sum(env);

			for (int a = 1; a < n; a++) {
				for (int b = a+1; b < n+1; b++) {
					for (int c = b+1; c < n+2; c++) {
						sum += T[a][b][c][k];
					}
				}
			}

			model.add(sum == TD[k]);
			/* destroy the sum object */
			sum.end();
		}

		/*  changes in permutation by transposition */
		/* i < a or i > c */
		for (i = 1; i < n+1; i++) {
			for (j = 1; j < n+1; j++) {
				for (k = 1; k < n; k++) {

					IloExpr sum(env);
			    
					for (int a = i+1; a < n; a++) {
						for (int b = a+1; b < n+1; b++) {
							for (int c = b+1; c < n+2; c++) {
								sum += T[a][b][c][k];
							}
						}
					}
					
					for (int a = 1; a < n; a++) {
						for (int b = a+1; b < n+1; b++) {
							for (int c = b+1; c < i+1; c++) {
								sum += T[a][b][c][k];
							}
						}
					}

					sum += B[i][j][k-1] + (1 - ZD[k]) - B[i][j][k];

					model.add(sum <= 1);
					/* destroy the sum object */
					sum.end();

				}
			}
		}

		/* a <= i < a + c - b */
		for (int a = 1; a < n+2; a++) {
			for (int b = 1; b < n+2; b++) {
				for (int c = 1; c < n+2; c++) {
					for (i = a; i <= a+c-b-1; i++) {
						for (j = 1; j < n+1; j++) {
							for (k = 1; k < n; k++) {
								if ((a < b) && (b < c)) {
									model.add(T[a][b][c][k] + B[b-a+i][j][k-1] - B[i][j][k] <= 1);
								}
							}
						}
					}
				}
			}
		}

		/* a + c - b <= i <= b */
		for (int a = 1; a < n+2; a++) {
			for (int b = 1; b < n+2; b++) {
				for (int c = 1; c < n+2; c++) {
					for (i = a+c-b; i <= c-1; i++) {
						for (j = 1; j < n+1; j++) {
							for (k = 1; k < n; k++) {
								if ((a < b) && (b < c)) {
									model.add(T[a][b][c][k] + B[b-c+i][j][k-1] - B[i][j][k] <= 1);
								}
							}
						}
					}
				}
			}
		}
		/* End Transposition Constraints */

		/* Reversal Constraints */
		/* at most one reversal is done at each step */
		for (k = 0; k < n; k++) {
			IloExpr sum(env);

			for (int a = 1; a < n; a++) {
				for (int b = a+1; b < n+1; b++) {
					sum += R[a][b][k];
				}
			}

			model.add(sum == RD[k]);
			/* destroy the sum object */
			sum.end();
		}

		/*  changes in permutation by reversal */
		/* i < a or i > b */
		for (i = 1; i < n+1; i++) {
			for (j = 1; j < n+1; j++) {
				for (k = 1; k < n; k++) {

					IloExpr sum(env);
			    
					for (int a = i+1; a < n; a++) {
						for (int b = a+1; b < n+1; b++) {
							sum += R[a][b][k];
						}
					}
					
					for (int a = 1; a < n; a++) {
						for (int b = a+1; b < i; b++) {
							sum += R[a][b][k];
						}
					}

					sum += B[i][j][k-1] + (1 - ZD[k]) - B[i][j][k];

					model.add(sum <= 1);
					/* destroy the sum object */
					sum.end();

				}
			}
		}

		/* a <= i <= b */
		for (int a = 1; a < n; a++) {
			for (int b = a+1; b < n+1; b++) {
				for (i = a; i <= b; i++) {
					for (j = 1; j < n+1; j++) {
						for (k = 1; k < n; k++) {
							model.add(R[a][b][k] + B[b+a-i][j][k-1] - B[i][j][k] <= 1);
						}
					}
				}
			}
		}
		/* End Reversal Constraints */

		/* Transposition+Reversal Constraints */
		/* if kth permutation does not alter the permutation,
		   none of subsequent will do so */
		model.add(ZD[0] == 1);
		for (k = 1; k < n; k++) {
			model.add(ZD[k] <= ZD[k-1]);
		}

		/* At most one operation (a reversal or a transposition 
		   is done */
		for (k = 1; k < n; k++) {
			model.add(TD[k] + RD[k] == ZD[k]);
		}

		/* End Transposition+Reversal Constraints */

		/* Bounds */
		IloExpr obj(env);

		for (k = 1; k < n; k++) {
			obj += ZD[k];
		}

		model.add(obj <= ub);
		model.add(obj >= lb);

		/* Solving the problem - Bottom-Up */
		for (int lim = lb; lim <= ub; lim++) {
			IloModel m(env);

			m.add(model);
			m.add(obj == lim);

			IloCP cp(env);
			cp.extract(m);

			if (cp.solve()) {
				cp.out() << std::endl ;
				cp.out() << "Optimal value: ";
				dist = cp.getValue(obj);
				cp.out() <<  dist << std::endl;
				break;
			}
		}

		obj.end();
	}
	catch (IloException &ex) {
		env.out() << "Error: " << ex.getMessage() << std::endl;
		return FAILURE;
	}

	env.end();

	return dist;

}
/* trans_rev_dist_csp */
/* trans_rev_dist */


/* EOF */
