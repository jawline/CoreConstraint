#include <stdio.h>
#include <iostream>
#include "solver.h"
#include "parser.h"

using namespace Simplex;
using namespace Constraints;
using namespace std;

int main(int argc, char** argv) {

	if (argc != 2) {
		printf("Usage: %s max problem\n", argv[0]);
		return -1;
	}

	Problem p;
	
	parserInit();

	if (!parseString(p, argv[1])) {
		return -1;
	}

	parserFree();

	Simplex::Solver::setLogging(true);

	std::cout << p.toString();
	return 0;
}