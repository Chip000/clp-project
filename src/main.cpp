#include <iostream>
#include <sstream>

using namespace std;


/*
 * usage: prints the usage message
 */
void usage(const char name[])
{
	cout << "USAGE:\n";
	cout << "======\n\n";

	cout << name << " <m> <b> <p>\n\n";

	cout << "- m is the model for the problem.\n";
	cout << "* trans     - sorting by transposition.\n";
	cout << "* rev       - sorting by reversal.\n";
	cout << "* trans_rev - sorting by transpositions+reversals.\n";
	cout << "\n";

	cout << "- b is the type of bounds for the problem.\n";
	cout << "* def    - default bound.\n";
	cout << "* rev_br - reversal breakpoint bound.\n";
	cout << "* tra_br - transposition breakpoint bound.\n";
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

	const char *model = argv[1];
	const char *btype = argv[2];
	const char *char_perm = argv[3];
	int *perm;
	int perm_size;

	if (argc < 4) {
		usage(argv[0]);
		return 1;
	}

	cout << "Model: " << model << endl;
	cout << "Bound type: " << btype << endl;
	cout << "Permutation: " << char_perm << endl;

	perm = perm2int(char_perm, perm_size);

	/* 
	 * TODO
	 */

	/* desallocate the memory */
	delete[] perm;

	return 0;

}

/* EOF */
