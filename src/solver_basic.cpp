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

void Solver::findBasic(Table& instance, int* rowBasicInfo) {

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
