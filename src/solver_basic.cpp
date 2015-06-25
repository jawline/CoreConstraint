#include "solver.h"
#include <stdio.h>
#include <string.h>

using namespace Simplex;

int Solver::findBasicRow(Table& instance, int col) {
	unsigned int count = 0;
	int row;

	for (unsigned int i = 1; i < instance.getNumRows(); i++) {
		if (instance.getField(i, col) != 0) {
			count++;
			row = i;
		}
	}

	return count == 1 ? row : -1;
}

void Solver::findBasicInfo(Table& instance, int* rowBasicInfo) {

	for (unsigned int i = 0; i < instance.getNumRows(); i++) {
		rowBasicInfo[i] = -1;
	}

	for (unsigned int i = 1; i < instance.getNumColumns(); i++) {
		int columnBasicRow = findBasicRow(instance, i);
		if (columnBasicRow != -1) {
			rowBasicInfo[columnBasicRow] = i;
		}
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

void Solver::tableBasicArtificialStep(Table& instance, int* rowBasis) {
	
	if (_excessiveLogging) {
		printf("------------------------------------------\n");
		printf("-             BASIC INFO                 -\n");
		printf("------------------------------------------\n");
	}
	
	//Calculate all existing basic data
	findBasicInfo(instance, rowBasis);

	//First row is the objective function, should have no basic variables
	for (unsigned int i = 1; i < instance.getNumRows(); i++) {

		//If a row has no basic column then insert an artificial variable to compensate
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
		
		if (_excessiveLogging) {
			double basicField = instance.getField(i, rowBasis[i]);
			double resultField = instance.getField(i, 0);
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
