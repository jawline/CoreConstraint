#include "solver.h"
#include <stdio.h>
#include <string.h>

using namespace Simplex;

/**
 * Return the ID of the pivot column or -1 if there is not pivot column
 */
int Solver::findPivotColumn(Table& instance, bool minimize) {

	//Check there are at least three columns (At least one variable, the objective variable, and the results columns)
	if (instance.getNumColumns() < 2) {
		return -1;
	}

	//Don't grab the first column, it shouldn't changed
	int cPivot = 1;
	double cPivotValue = instance.getField(0, 1);

	if (minimize) {
		//Never look at the first column, it shouldn't change
		for (unsigned int i = 1; i < instance.getNumColumns(); i++) {
			if (instance.getField(0, i) != 0 && instance.getField(0, i) > cPivotValue) {
				cPivot = i;
				cPivotValue = instance.getField(0, i);
			}
		}
		//If the columns objective value is >= 0 then it cannot be a pivot column
		return cPivotValue > 0 ? cPivot : -1;
	} else {
		//Never look at the first column, it shouldn't change
		for (unsigned int i = 1; i < instance.getNumColumns(); i++) {
			if (instance.getField(0, i) != 0 && instance.getField(0, i) < cPivotValue) {
				cPivot = i;
				cPivotValue = instance.getField(0, i);
			}
		}
		//If the columns objective value is >= 0 then it cannot be a pivot column
		return cPivotValue < 0 ? cPivot : -1;
	}
}

/**
 * The pivot row is the row with the minimal positive result / pivot column ratio
 */
int Solver::findPivotRow(Table& instance, int column) {

	if (instance.getNumRows() < 2) {
		printf("ERROR: no pivot possible, table poorly formed\n");
		return -1;
	}

	int resultsColumn = 0;
	
	int cPivot = 1;
	double cPivotR = findRatio(instance, 1, column, resultsColumn);
	
	//Find the row to be used as the pivot, excluding the objective function
	for (unsigned int i = 1; i < instance.getNumRows(); i++) {
		double ratio = findRatio(instance, i, column, resultsColumn);
		if (ratio != 0 && ratio > 0) {
			if (cPivotR <= 0 || ratio < cPivotR) {
				cPivot = i;
				cPivotR = ratio;
			}
		}
	}

	if (_excessiveLogging) {
		printf("PivotR: %i %f\n", cPivot, cPivotR);
	}

	return cPivotR > 0 ? cPivot : -1;
}
