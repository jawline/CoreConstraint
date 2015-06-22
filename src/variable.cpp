#include "variable.h"

using namespace Constraints;

Variable::Variable() {}
Variable::Variable(std::string const& name, bool isSigned, unsigned int index) {
	_name = name;
	_isSigned = isSigned;
	_variableIndex = index;
}

Variable::~Variable() {}

std::string Variable::toString() const {
	return _name;
}

bool Variable::isSigned() const {
	return _isSigned;
}

unsigned int Variable::getVariableIndex() const {
	return _variableIndex;
}

bool Variable::equals(Variable const& other) const {
	return _variableIndex == other._variableIndex;
}