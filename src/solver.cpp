#include "solver.h"
#include <stdio.h>
#include <string.h>

using namespace Simplex;

bool Solver::_excessiveLogging = false;

double Solver::findRatio(Table& instance, int row, int column, int resCol) {
	double resultField = instance.getField(row, column);
	//TODO: Verify this is correct
	//Avoids divide by zero
	return resultField != 0 ? instance.getField(row, resCol) / resultField : 0;
}

void Solver::makeRowUnit(Table& instance, int row, int col) {
	instance.divideRow(row, instance.getField(row, col));
}

void Solver::makeOtherRowsUnit(Table& instance, int baseRow, int col) {
	for (unsigned int i = 0; i < instance.getNumRows(); i++) {
		if (i != baseRow && instance.getField(i, col) != 0) {
			instance.subtractRow(i, baseRow, instance.getField(i, col));
		}
	}
}

void Solver::doPivot(Table& instance, int* basis, unsigned int pivotR, unsigned int pivotC) {		
	double ratio = findRatio(instance, pivotR, pivotC, 0);
	makeRowUnit(instance, pivotR, pivotC);
	makeOtherRowsUnit(instance, pivotR, pivotC);
	basis[pivotR] = pivotC;
	
	if (_excessiveLogging) {
		printf("DEBUG: Pivot Column: %i\n", pivotC);
		printf("DEBUG: Pivot Row: %i\n", pivotR);
		printf("DEBUG: Pivot Ratio: %f\n", ratio);
		instance.print();
	}
}

bool Solver::pivotTable(Table& instance, int* rowBasis, bool minimize) {
	int pivotC, iterations = 0;

	while ((pivotC = findPivotColumn(instance, minimize)) != -1) {

		int pivotR = findPivotRow(instance, pivotC);
		if (pivotR == -1) {
			if (_excessiveLogging) {
				printf("DEBUG: PivotR returns -1, table is unsolvable %i %i\n", pivotC, pivotR);
				instance.print();
			}
			return false;
		}
		
		iterations++;

		if (_excessiveLogging) {
			printf("DEBUG: Operation Number: %i\n", iterations);
		}

		doPivot(instance, rowBasis, pivotR, pivotC);
	}

	if (_excessiveLogging) {
		printf("Could not find pivotC %i\n", pivotC);
		instance.print();
		printf("-----------------------------------------\n");
	}

	return true;
}

bool Solver::solveTable(Table& instance, SimplexResult& results) {
	_lastArtificial = 0;

	if (_excessiveLogging) {
		printf("DEBUG: Entered with table\n");
		instance.print();
	}

	int* rowBasis = new int[instance.getNumRows()];

	//Find the columns with only one `1` or insert artificial variables
	tableBasicArtificialStep(instance, rowBasis);
	
	/* 
	 * If there are any artificial variables inserted then this min step will attempt to find
	 * if the table is solvable by minimizing them. If it is solvable it will find an initial
	 * feasible solution which can then be pivoted by the second phase
	 */
	if (!artificialMinStep(instance, rowBasis)) {
		return false;
	}
	
	if (!pivotTable(instance, rowBasis)) {
		return false;
	}
	
	handleFinalBasicData(instance, rowBasis);
	results = SimplexResult(instance, rowBasis);
	delete[] rowBasis;

	return true;
}

void Solver::setLogging(bool enabled) {
	_excessiveLogging = enabled;
}
