#include "tableau.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

using namespace Simplex;

Table::Table() {
	_columns = nullptr;
	_numColumns = 0;

	_rowData = nullptr;
	_numRows = 0;
}

Table::Table(Table const& other) {
	copyFrom(other);
}

Table::~Table() {
	empty();
}

void Table::copyFrom(Table const& other) {
	_numColumns = other.getNumColumns();
	_numRows = other.getNumRows();
	_rowData = nullptr;
	_columns = nullptr;

	if (getNumColumns()) {
		_columns = new Column[getNumColumns()];
		for (unsigned int i = 0; i < getNumColumns(); i++) {
			_columns[i] = other._columns[i];
		}
	}

	if (getNumRows() * getNumColumns()) {
		_rowData = new double[getNumRows() * getNumColumns()];
		for (unsigned int i = 0; i < getNumColumns() * getNumRows(); i++) {
			_rowData[i] = other._rowData[i];
		}
	}
}

int Table::getColumnId(std::string const& name) const {
	for (int i = 0; i < _numColumns; i++) {
		if (_columns[i].getName().compare(name) == 0) {
			return i;
		}
	}
	return -1;
}

Column* Table::getColumn(std::string const& name) const {
	int col = getColumnId(name);
	assert(col != -1);
	return getColumn(col);
}

void Table::removeArtificials() {
	//TODO: Remove artificials
	//TODO: This just nulls all values in artificial columns so they dont effect the results. It sucks
	//Remove instead
	for (unsigned int i = 0; i < getNumColumns(); i++) {
		if (getColumn(i)->isArtificial()) {
			for (unsigned int j = 0; j < getNumRows(); j++) {
				setField(j, i, 0);
			}
			getColumn(i)->setArtificial(false);
		}
	}
}

Column* Table::getColumn(int i) const {
	return &_columns[i];
}

int Table::addColumn(std::string const& name) {
	return addColumn(name, false);
}

std::vector<int> Table::getArtificialColumnList() const {
	std::vector<int> results;
	for (unsigned int i = 0; i < getNumColumns(); i++) {
		if (_columns[i].isArtificial()) {
			results.push_back(i);
		}
	}
	return results;
}

int Table::addColumn(std::string const& name, bool artificial) {

	//If it already exists just update its artificialness and return it
	if (getColumnId(name) != -1) {
		_columns[getColumnId(name)].setArtificial(artificial);
		return getColumnId(name);
	}

	//Allocate memory and copy existing columns
	Column* newColumns = new Column[getNumColumns() + 1];
	for (unsigned int i = 0; i < getNumColumns(); i++) {
		newColumns[i] = _columns[i];
	}
	
	//Free existing data and set new data
	if (_columns) {
		delete[] _columns;
	}
	_columns = newColumns;
	
	//Initialise new column
	_columns[getNumColumns()] = Column(name);
	_columns[getNumColumns()].setArtificial(artificial);
	
	//Expand existing row data
	expandRows(getNumColumns() + 1);
	return _numColumns++;
}

void Table::addRow() {
	
	//Allocate memory and copy over existing rows
	double* newRowData = new double[(getNumRows() + 1) * _numColumns];
	
	//Copy existing data over, null the rest
	for (unsigned int i = 0; i < (getNumRows() + 1) * getNumColumns(); i++) {
		newRowData[i] = i < getNumRows() * getNumColumns() ? _rowData[i] : 0;
	}
	
	//Set new rows to 0
	//Free old data and set new row data
	if (_rowData) {
		delete[] _rowData;
	}
	_rowData = newRowData;
	_numRows++;
}

void Table::expandRows(int newNumColumns) {
	
	//Allocate new data
	double* newRowData = new double[getNumRows() * newNumColumns];
	
	//Copy the existing rows
	for (unsigned int column = 0; column < newNumColumns; column++) {
		for (unsigned int row = 0; row < getNumRows(); row++) {
			if (column < getNumColumns()) {
				newRowData[(row * newNumColumns) + column] = getField(row, column);
			} else {
				newRowData[(row * newNumColumns) + column] = 0;
			}
		}
	}
	
	//Free old data and set new data
	if (_rowData) {
		delete[] _rowData;
	}

	_rowData = newRowData;
}

int Table::getNumRows() const {
	return _numRows;
}

int Table::getNumColumns() const {
	return _numColumns;
}

unsigned int Table::getCurrentRow() const {
	return _numRows >= 1 ? _numRows - 1 : 0;
}

double* Table::getRowData(unsigned int rowNum) const {
	assert(rowNum < _numRows);
	return _rowData + (_numColumns * rowNum);
}

double Table::getField(unsigned int row, unsigned int column) const {
	return *(getRowData(row) + column);
}

void Table::setField(unsigned int row, unsigned int column, double val) {
	assert(row < getNumRows());
	assert(column < getNumColumns());
	*(getRowData(row) + column) = val;
}

double Table::getField(unsigned int row, std::string const& name) const {
	int col = getColumnId(name);
	assert(col != -1);
	return getField(row, col);
}

void Table::setField(unsigned int row, std::string const& name, double val) {
	int col = getColumnId(name);
	assert(col != -1);
	setField(row, col, val);
}

void Table::print() {
	for (unsigned int i = 0; i < _numColumns; i++) {
		printf("| %s |", _columns[i].getName().c_str());
	}
	printf("\n");
	for (unsigned int row = 0; row < _numRows; row++) {
		for (unsigned int column = 0; column < _numColumns; column++) {
			printf("| %f |", _rowData[row * _numColumns + column]);
		}
		printf("\n");
	}
}

void Table::subtractRow(unsigned int r1, unsigned int r2, double scalar) {
	for (unsigned int i = 0; i < getNumColumns(); i++) {
		setField(r1, i, getField(r1, i) - (getField(r2, i) * scalar));
	}
}

void Table::multiplyRow(unsigned int r1, double scalar) {
	for (unsigned int i = 0; i < getNumColumns(); i++) {
		setField(r1, i, getField(r1, i) * scalar);
	}
}

void Table::divideRow(unsigned int r1, double divisor) {
	multiplyRow(r1, 1.0 / divisor);
}

void Table::subtractRow(unsigned int r1, unsigned int r2) {
	subtractRow(r1, r2, 1);
}

void Table::empty() {
	_numRows = 0;
	_numColumns = 0;
	if (_rowData) {
		delete[] _rowData;
		_rowData = nullptr;
	}
	if (_columns) {
		delete[] _columns;
		_columns = nullptr;
	}
}
