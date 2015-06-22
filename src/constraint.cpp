#include "constraint.h"
#include "problem_constants.h"
#include <math.h>

using namespace Constraints;

const char* Constraint::ComparisonTypeStrings[NumComparisonTypes] = {"invalid", "=", "!=", "<", ">", ">=", "<="};
unsigned int slackd = 0;

Constraint::Constraint() {
	_value = 0;
	_type = Equal;
}

Constraint::Constraint(double value) {
	_value = value;
	_type = Equal;
}

Constraint::~Constraint() {}

void Constraint::addItem(Variable const& var, double multiplier) {
	_items.push_back(ConstraintItem(var, multiplier));
}

double Constraint::getResult() const {
	return _value;
}

void Constraint::setResult(double endValue) {
	_value = endValue;
}

void Constraint::setComparisonType(ComparisonType const& type) {
	_type = type;
}

std::string Constraint::toString() const {
	std::string result;
	if (_items.size()) {
		for (unsigned int i = 0; i < _items.size(); i++) {
			if (i) {
				if (_items[i].second > 0) {
					result += "+ ";
				} else {
					result += "- ";
				}
			}
			result += std::to_string(fabs(_items[i].second));
			result += _items[i].first.toString();
			result += " ";
		}
		result += ComparisonTypeStrings[_type];
		result += " (";
		result += std::to_string(_type);
		result += ") ";
		result += std::to_string(_value);
	} else {
		result = "Empty Constraint";
	}
	return result;
}

void Constraint::addToTable(Simplex::Table& table, ComparisonType typeOverride) const {
	table.addRow();
	
	for (unsigned int i = 0; i < _items.size(); i++) {
		table.addColumn(_items[i].first.toString());
		table.setField(table.getCurrentRow(), _items[i].first.toString(), _items[i].second);
	}
	
	ComparisonType localType = typeOverride == Invalid ? _type : typeOverride;
	double localValue = _value;
	
	if (localType == GreaterThan) {
		localType = GreaterThanOrEqual;
		localValue += 1;
	}
	
	if (localType == LessThan) {
		localType = LessThanOrEqual;
		localValue -= 1;
	}

	std::string name;

	switch (localType) {
		case LessThanOrEqual:
			name = std::string("slack") + std::to_string(slackd);
			table.addColumn(name);
			table.setField(table.getCurrentRow(), name, 1);
			slackd++;
			break;
		case GreaterThanOrEqual:
			name = std::string("slack") + std::to_string(slackd);
			table.addColumn(name);
			table.setField(table.getCurrentRow(), name, -1);
			slackd++;
			break;
		default:
			printf("%s NOT HANDLED YET\n", ComparisonTypeStrings[_type]);
			break;
	}

	table.setField(table.getCurrentRow(), ProblemConstants::cResultColumnName, localValue);
}

Constraint& Constraint::add(Constraint const& other) {
	for (unsigned int i = 0; i < other._items.size(); i++) {
		bool exists = false;

		for (unsigned int j = 0; j < _items.size(); j++) {
			if (_items[j].first.equals(other._items[i].first)) {
				_items[j].second += other._items[i].second;
				exists = true;
			}
		}

		if (!exists) {
			_items.push_back(other._items[i]);
		}
	}

	_value += other._value;
	return *this;
}

Constraint& Constraint::minus(Constraint const& other) {
	for (unsigned int i = 0; i < other._items.size(); i++) {
		bool exists = false;

		for (unsigned int j = 0; j < _items.size(); j++) {
			if (_items[j].first.equals(other._items[i].first)) {
				_items[j].second -= other._items[i].second;
				exists = true;
			}
		}

		if (!exists) {
			_items.push_back(ConstraintItem(other._items[i].first, -other._items[i].second));
		}
	}

	_value -= other._value;
	return *this;
}

Constraint& Constraint::multiply(Constraint const& other) {
	printf("TODO: Multiply\n");
	return *this;
}

Constraint& Constraint::divide(Constraint const& other) {
	printf("TODO: Divide\n");
	return *this;
}

Constraint& Constraint::scale(double scalar) {
	for (unsigned int i = 0; i < _items.size(); i++) {
		_items[i].second *= scalar;
	}
	_value *= scalar;
	return *this;
}

void Constraint::buildResult(ComparisonType type, double endValue) {
	setComparisonType(type);
	setResult(getResult() + endValue);
}

ComparisonType Constraint::getComparisonType() const {
	return _type;
}