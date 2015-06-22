#ifndef _SIMPLEX_TABLAEU_DEF_H_
#define _SIMPLEX_TABLAEU_DEF_H_
#include "column.h"
#include <vector>

namespace Simplex {

	class Table {
	private:
		Column* _columns;
		double* _rowData;
		int _numColumns;
		int _numRows;

		/**
		 * Expand the rowdata after a new column has been added
		 */
		void expandRows(int numNewColumns);
		
		/**
		 * Copy this data from another
		 */
		void copyFrom(Table const& other);
	public:
		Table();
		Table(Table const& other);
		~Table();

		int getColumnId(std::string const& name) const;
		Column* getColumn(std::string const& name) const;
		Column* getColumn(int i) const;

		int getNumRows() const;
		int getNumColumns() const;

		int addColumn(std::string const& column);
		int addColumn(std::string const& column, bool artificial);
		void addRow();

		unsigned int getCurrentRow() const;
		double* getRowData(unsigned int row) const;
		
		double getField(unsigned int row, unsigned int col) const;
		double getField(unsigned int row, std::string const& column) const;

		/**
		 * Set the table value as the specified row & column
		 */
		void setField(unsigned int row, unsigned int col, double value);
		
		/**
		 * Set the value of the column with the specified name at the specified row
		 */
		void setField(unsigned int row, std::string const& column, double value);

		/**
		 * Subtract a multiple of one row from another
		 */
		void subtractRow(unsigned int r1, unsigned int r2, double scalar);
		
		/**
		 * Subtract one row from another
		 */
		void subtractRow(unsigned int r1, unsigned int r2);

		/**
		 * Multiply one row with another
		 */
		void multiplyRow(unsigned int r1, double scalar);
		
		/**
		 * Divide one row by another
		 */
		void divideRow(unsigned int r1, double divisor);

		/**
		 * Return a list of artificial variables
		 */
		std::vector<int> getArtificialColumnList() const;

		/**
		 * Clear the table, deleting all rows and columns
		 */
		void empty();
		
		/**
		 * Remove artificial variable columns from the table
		 * TODO: Currently just nulls them rather than actually freeing the memory, is it worth freeing?
		 */
		void removeArtificials();

		/**
		 * Print the table info to stdout
		 */
		void print();

		/**
		 * Override = operator to copy data rather than using the same pointers (Avoids double free)
		 */
		Table& operator=(Table const& other) {
			copyFrom(other);
			return *this;
		}
	};
}

#endif //_SIMPLEX_TABLAEU_DEF_H_
