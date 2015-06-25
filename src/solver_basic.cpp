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

//TODO: Could be more efficient. The row->col->row search could be turned into just a row->col search
int Solver::findBasic(Table& instance, int row) {

	for (unsigned int i = 1; i < instance.getNumColumns(); i++) {
		if (findBasicRow(instance, i) != -1 && instance.getField(row, i) == 1) {
			return i;
		}
	}

	return -1;
}
