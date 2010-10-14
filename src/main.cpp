#include <iostream>
#include <sstream>
#include <ctime>
#include <cstdio>
#include "../include/cp.h"

using namespace std;

#define COP "cop"
#define CSP "csp"

#define TRA "trans"
#define REV "rev"
#define T_R "trans_rev"

/*
 * usage: prints the usage message
 */
void usage(const char name[])
{
	cout << "USAGE:\n";
	cout << "======\n\n";

	cout << name << " <t> <m> <b> <p>\n\n";

	cout << "- t is the type for the problem.\n";
	cout << "* " << COP << " - Constraint Optimization Problem.\n";
	cout << "* " << CSP << " - Constraint Satisfaction Problem.\n";
	cout << "\n";

	cout << "- m is the model for the problem.\n";
	cout << "* " << TRA << "     - sorting by transposition.\n";
	cout << "* " << REV << "       - sorting by reversal.\n";
	cout << "* " << T_R << " - sorting by transpositions+reversals.\n";
	cout << "\n";

	cout << "- b is the type of bounds for the problem.\n";
	cout << "* " << DEF << "    - default bound.\n";
	cout << "* " << TRA_BR << " - transposition breakpoint bound.\n";
	cout << "* " << REV_BR << " - reversal breakpoint bound.\n";
	cout << "* " << T_R_BR << " - transposition+reversal breakpoint bound.\n";
	cout << "\n";

	cout << "- p is the given permutation.\n";
	cout << "* Ex: \"1 2 3 4\".\n";
	cout << "\n";

	return;

} /* usage */


/*
 * perm2int: Returns an array of int representing the permutation and
 * its length
 */
int *perm2int(const char perm[], int &n)
{

	char *ptr;
	int tmp;
	int *ret;
	
	ptr = (char *) perm;
	n = 0;
	while (stringstream(ptr) >> tmp) {
		n++;
		ptr +=2; /* ignoring the space */
	}

	ptr = (char *) perm;
	ret = new int[n];
	tmp = 0;
	while (stringstream(ptr) >> ret[tmp]) {
		ptr +=2; /* ignoring the space */
		tmp ++;
	}
	
	return ret;

} /* perm2int */

int main(int argc, const char *argv[])
{

	const char *theory = argv[1];
	const char *model = argv[2];
	const char *btype = argv[3];
	const char *char_perm = argv[4];
	int *perm;
	int perm_size;
	int ret;

	clock_t c_begin;
	clock_t c_end;

	c_begin = clock();

	if (argc < 5) {
		usage(argv[0]);
		return 1;
	}

	cout << ">> Type: "<< theory << endl;
	cout << ">> Model: " << model << endl;
	cout << ">> Bound type: " << btype << endl;
	cout << ">> Permutation: " << char_perm << endl;

	perm = perm2int(char_perm, perm_size);

	/* Creating the object problem */
	CP prob(perm, perm_size, btype);

	cout << endl;

	ret = -1;

	if (strcmp(TRA, model) == 0) {
		if (strcmp(COP, theory) == 0) {
			ret = prob.trans_dist_cop();
		}
		else if (strcmp(CSP, theory) == 0) {
			ret = prob.trans_dist_csp();
		}
		else {
			cerr << ">> ERROR: Invalid type" << endl << endl;
			usage(argv[0]);
		}
	}
	else if (strcmp(REV, model) == 0) {
		if (strcmp(COP, theory) == 0) {
			ret = prob.rev_dist_cop();
		}
		else if (strcmp(CSP, theory) == 0) {
			ret = prob.rev_dist_csp();
		}
		else {
			cerr << ">> ERROR: Invalid type" << endl << endl;
			usage(argv[0]);
		}
	}
	else if (strcmp(T_R, model) == 0) {
		if (strcmp(COP, theory) == 0) {
			ret = prob.trans_rev_dist_cop();
		}
		else if (strcmp(CSP, theory) == 0) {
			ret = prob.trans_rev_dist_csp();
		}
		else {
			cerr << ">> ERROR: Invalid type" << endl << endl;
			usage(argv[0]);
		}
	}
	else {
		cerr << ">> ERROR: Model not found" << endl << endl;
		usage(argv[0]);
	}

	if (ret == 0) {
		cout << "Optimal value: 0" << endl;
	}

	c_end = clock();

	fprintf(stdout, "Total Time: %.4f\n",
		(double) (c_end - c_begin) / CLOCKS_PER_SEC);

	cout << endl;

	/* desallocate the memory */
	delete[] perm;

	return 0;

}

/* EOF */
