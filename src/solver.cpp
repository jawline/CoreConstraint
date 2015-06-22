#include "solver.h"
#include <stdio.h>
#include <string.h>

using namespace Simplex;

bool Solver::_excessiveLogging = false;
unsigned int Solver::_lastArtificial = 0;

int Solver::findBasicRow(Table& instance, int col) {
	unsigned int count = 0;
	int row;

	//Assume row 0 is the results row
	for (unsigned int i = 1; i < instance.getNumRows(); i++) {
		if (instance.getField(i, col) != 0) {
			count++;
			row = i;
		}
	}
	return count == 1 ? row : -1;
}

//TODO: Could be more efficient. The row->col->row search could be turned into just a row->col search
int Solver::findBasic(Table& instance, int row) {

	//-1 excludes the result row
	for (unsigned int i = 1; i < instance.getNumColumns(); i++) {
		if (findBasicRow(instance, i) != -1 && instance.getField(row, i) == 1) {
			return i;
		}
	}
	return -1;
}

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

double Solver::findRatio(Table& instance, int row, int column, int resCol) {
	double resultField = instance.getField(row, column);
	//TODO: Verify this is correct
	//Avoids divide by zero
	return resultField != 0 ? instance.getField(row, resCol) / resultField : 0;
}

bool Solver::artificialColumnsInBasis(int* basis, unsigned int numRows, std::vector<int> const& artificialColumns) {
	for (unsigned int i = 0; i < numRows; i++) {
		for (unsigned int j = 0; j < artificialColumns.size(); j++) {
			if (basis[i] == artificialColumns[j]) {
				return true;
			}
		}
	}
	return false;
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

void Solver::setupArtificialTable(Table& instance, Table& original, std::vector<int> const& artificialVariables) {
	//TODO: store the top row instead, this is crazy ineficient
	original = instance;
	for (unsigned int i = 0; i < instance.getNumColumns(); i++) {
		instance.setField(0, i, 0);
	}
	for (unsigned int i = 0; i < artificialVariables.size(); i++) {
		instance.setField(0, artificialVariables[i], -1);
	}
}

void Solver::restoreTable(Table& instance, Table& original) {
	for (unsigned int i = 0; i < instance.getNumColumns(); i++) {
		instance.setField(0, i, original.getField(0, i));
	}
	instance.removeArtificials();
}


void Solver::findBasicData(Table& instance, int* rowBasis) {
	
	if (_excessiveLogging) {
		printf("------------------------------------------\n");
		printf("-             BASIC INFO                 -\n");
		printf("------------------------------------------\n");
	}

	//First row is the objective function, should have no basic variables
	for (unsigned int i = 1; i < instance.getNumRows(); i++) {
		
		rowBasis[i] = findBasic(instance, i);
		
		if (rowBasis[i] == -1) {
			int col = instance.addColumn(std::string("artificial") + std::to_string(_lastArtificial++), true);
			instance.setField(i, col, 1);
			rowBasis[i] = col;
			
			if (_excessiveLogging) {
				printf("DEBUG: Failed to find basic variable for row %i\n", i);
				printf("DEBUG: Creating artificial variable for row %i\n", i);
				instance.print();
			}
		}

		double basicField = instance.getField(i, rowBasis[i]);
		double resultField = instance.getField(i, 0);
		
		if (_excessiveLogging) {
			printf("DEBUG: Row %i: Col %i is basic (Solution: %f/%f -> %f)\n",
				i,
				rowBasis[i],
				instance.getField(i, rowBasis[i]),
				instance.getField(i, 0),
				resultField == 0 ? 0 : basicField / resultField);
		}
	}
	
	if (_excessiveLogging) {
		printf("------------------------------------------\n");
	}
}

void Solver::handleFinalBasicData(Table& instance, int* rowBasis) {
	if (_excessiveLogging) {
		printf("------------------------------------------\n");
		printf("-              FINAL BASIC               -\n");
		printf("------------------------------------------\n");
		for (unsigned int i = 0; i < instance.getNumRows(); i++) {
			if (rowBasis[i] != -1) {
				printf("%s: %f\n",
					instance.getColumn(i)->getName().c_str(), 
					instance.getField(i, 0));
			} else {
				printf("Row %i unmapped\n", i);
			}
		}
		printf("------------------------------------------\n");
	}
}

bool Solver::allArtificialsZero(Table const& instance, std::vector<int> const& artificialVariables) {
	for (unsigned int i = 0; i < artificialVariables.size(); i++) {
		if (instance.getField(0, artificialVariables[i]) != 0) {
			return false;
		}
	}
	return true;
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

bool Solver::artificialMinStep(Table& instance, int* rowBasis) {
	std::vector<int> artificialVariables = instance.getArtificialColumnList();
	unsigned int numArtificials = artificialVariables.size();
	
	if (numArtificials) {
		Table original = instance;
		setupArtificialTable(instance, original, artificialVariables);
		
		if (_excessiveLogging) {
			printf("DEBUG: Changed to artificial table\n");
			instance.print();
		}

		doPivot(instance, rowBasis, findBasicRow(instance, artificialVariables[0]), artificialVariables[0]);

		if (!pivotTable(instance, rowBasis, true)) {
			return false;
		}

		if (artificialColumnsInBasis(rowBasis, instance.getNumRows(), artificialVariables)) {
			printf("DEBUG: Artificial columns still in basis, unsolvable\n");
			return false;
		}

		if (instance.getField(0,0) != 0) {
			printf("DEBUG: Result of artificial minimization != 0, not solvable\n");
			return false;
		}

		handleFinalBasicData(instance, rowBasis);

		restoreTable(instance, original);
		
		if (_excessiveLogging) {
			printf("DEBUG: Stripped artificials\n");
			instance.print();
		}
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
	findBasicData(instance, rowBasis);
	
	/* 
	 * If there are any artificial variables inserted then this min step will attempt to find
	 * if the table is solvable by minimizing them. If it is solvable it will find an initial
	 * feasible solution which can then be pivoted by the second phase
	 */
	if (!artificialMinStep(instance, rowBasis)) {
		return false;
	}
	
	if (!pivotTable(instance, rowBasis, false)) {
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
