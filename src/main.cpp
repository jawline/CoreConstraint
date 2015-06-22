/*#include <stdio.h>
#include "simplex/parser.h"
#include "simplex/solver.h"

using namespace Simplex;

int main(int argc, char** argv) {

	if (argc != 2) {
		printf("Usage: %s max problem\n", argv[0]);
		return -1;
	}

	Table instance;
	SimplexResult results;
	
	parserInit();

	if (parseString(instance, argv[1])) {
		instance.print();
	} else {
		printf("Failed to parse table. Exit\n");
		return -1;
	}

	Solver::solveTable(instance, results);

	printf("Results: %f\n", results.result);
	parserFree();
	return 0;
}*/

#include <stdio.h>
#include <iostream>
#include "problem.h"
#include "solver.h"

using namespace Constraints;

int main(int argc, char** argv) {
	printf("CoreConstraints\n");

	Problem p;
	Variable a = p.createVariable("A");

	Constraint d;
	
	d.addItem(a, 1);
	d.setComparisonType(LessThan);
	d.setResult(10);

	Constraint q;

	q.addItem(a, 1);
	q.setComparisonType(GreaterThanOrEqual);
	q.setResult(5);

	p.addConstraint(d);
	p.addConstraint(q);

	std::cout << p.toString();
	return -1;
}
