#include "solver.h"
#include <stdio.h>
#include <string.h>

using namespace Simplex;

unsigned int Solver::_lastArtificial = 0;

void Solver::setupArtificialTable(Table& instance, Table& original, std::vector<int> const& artificialVariables) {
	//TODO: store the top row instead, this is crazy awful
	original = instance;
	for (unsigned int i = 0; i < instance.getNumColumns(); i++) {
		instance.setField(0, i, 0);
	}
	for (unsigned int i = 0; i < artificialVariables.size(); i++) {
		instance.setField(0, artificialVariables[i], -1);
	}
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

void Solver::restoreObjective(Table& instance, Table& original) {
	for (unsigned int i = 0; i < instance.getNumColumns(); i++) {
		instance.setField(0, i, original.getField(0, i));
	}
	instance.removeArtificials();
}

bool Solver::allArtificialsZero(Table const& instance, std::vector<int> const& artificialVariables) {
	for (unsigned int i = 0; i < artificialVariables.size(); i++) {
		if (instance.getField(0, artificialVariables[i]) != 0) {
			return false;
		}
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
		restoreObjective(instance, original);
		
		if (_excessiveLogging) {
			printf("DEBUG: Stripped artificials\n");
			instance.print();
		}
	}
	
	return true;
}
