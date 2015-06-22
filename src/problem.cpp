#include "problem.h"
#include "problem_constants.h"
#include "solver.h"

using namespace Constraints;
using namespace Simplex;

Problem::Problem() {
	_lastVariableId = 0;
}

Problem::~Problem() {}

Variable Problem::createVariable(std::string const& name) {
	_variables.push_back(Variable(name, true, _lastVariableId));
	return Variable(name, true, _lastVariableId++);
}

void Problem::addConstraint(Constraint const& constraint) {
	_constraints.push_back(constraint);
}

bool Problem::isSatisfiable() const {
	Table table;
	table.empty();
	table.addRow();
	table.addColumn(ProblemConstants::cResultColumnName);
	table.setField(table.getCurrentRow(), ProblemConstants::cResultColumnName, 0);
	
	for (unsigned int i = 0; i < _variables.size(); i++) {
		table.addColumn(_variables[i].toString());
		table.setField(table.getCurrentRow(), _variables[i].toString(), -1);
	}

	return simSat(table, 0);
}

std::string Problem::toString() const {
	std::string result;

	result = "Problem\n";

	if (_variables.size()) {
		for (unsigned int i = 0; i < _variables.size(); i++) {
			if (i) {
				result += ", ";
			}
			result += _variables[i].toString();
		}
		result += "\n";
	}

	for (unsigned int i = 0; i < _constraints.size(); i++) {
			result += _constraints[i].toString() + "\n";
	}

	if (_variables.size()) {
		for (unsigned int i = 0; i < _variables.size(); i++) {
			if (i) {
				result += ", ";
			}
			result += _variables[i].toString();
		}
		result += " >= 0\n";
	}

	printf("Satisfiable: %i\n", isSatisfiable());

	return result;
}

bool Problem::isSolvable(Simplex::Table& currentTable, unsigned int i) const {
	if (i == _constraints.size() - 1) {
		SimplexResult result;
		bool isSat = Solver::solveTable(currentTable, result);

		if (isSat) {
			for (unsigned int i = 0; i < currentTable.getNumColumns(); i++) {
				double v;
				if (result.getResult(i, v)) {
					printf("%s: %f\n", currentTable.getColumn(i)->getName().c_str(), v);
				}
			}
		}

		return isSat;
	}
	return simSat(currentTable, i+1);
}

bool Problem::simSat(Simplex::Table& currentTable, unsigned int i) const {
	if (_constraints[i].getComparisonType() == NotEqual) {
		//TODO: doesn't need to be a complete copy
		Table copy = currentTable;
		_constraints[i].addToTable(currentTable, GreaterThan);
		if (isSolvable(currentTable, i)) {
			return true;
		}
		_constraints[i].addToTable(copy, LessThan);
		return isSolvable(copy, i);
	}
	
	_constraints[i].addToTable(currentTable);
	return isSolvable(currentTable, i);
}
