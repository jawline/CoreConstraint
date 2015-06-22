#include "column.h"

using namespace Simplex;

Column::Column() {
	_name = "INVALID";
}

Column::Column(std::string const& name) {
	_name = name;
}

Column::Column(char const* name, size_t len) {
	_name = std::string(name, len);
}

Column::~Column() {
}

std::string Column::getName() const {
	return _name;
}

void Column::setArtificial(bool a) {
	_artificial = a;
}

bool Column::isArtificial() const {
	return _artificial;
}