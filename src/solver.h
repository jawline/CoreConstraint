#ifndef _TABLEAU_SOLVER_DEF_H_
#define _TABLEAU_SOLVER_DEF_H_
#include <vector>
#include "tableau.h"
#include "simplexresult.h"

namespace Simplex {

	class Solver {
	private:
		/**
		 * When enabled the solver will spit out a lot of debug info including the table at each pivot
		 */
		static bool _excessiveLogging;
		
		/**
		 * Used to give each artificial variable added a unique name
		 */
		static unsigned int _lastArtificial;
		
		/**
		 * Return the ID of the pivot column or -1 if there is not pivot column
		 */
		static int findPivotColumn(Table& instance, bool minimize);
		
		/**
		 * The pivot row is the row with the minimal positive result / pivot column ratio
		 */
		static int findPivotRow(Table& instance, int column);
		
		/**
		 * Find the basic row for a column
		 */
		static int findBasicRow(Table& instance, int col);
		
		/**
		 * Compute the basic columns for each row
		 */
		static void findBasicInfo(Table& instance, int* rowBasicInfo);
		
		/**
		 * Compute the row basic data for the table, inserting artificial variables if necissary
		 */
		static void tableBasicArtificialStep(Table& instance, int* rowBasis);
		
		/**
		 * Do a single pivot operation with the specified row and column
		 */
		static void doPivot(Table& instance, int* basis, unsigned int row, unsigned int col);
		
		/**
		 * Pivot the table finding either the max or min if solvable (Finds min if minimize = true)
		 */
		static bool pivotTable(Table& instance, int* rowBasicData, bool minimize = false);
		
		/**
		 * Called after the table is pivoted, can post process the final basic variable row data
		 */
		static void handleFinalBasicData(Table& instance, int* rowBasicData);
		
		/**
		 * Returns true if all the artificial rows have a result column value of 0
		 */
		static bool allArtificialsZero(Table const& instance, std::vector<int> const& artificialVariables);
		
		/**
		 * Returns true if there are artificial columns in the basis
		 */
		static bool artificialColumnsInBasis(int* basis, unsigned int numRows, std::vector<int> const& artificialColumns);
		
		/**
		 * Finds the minimization on the table with artificial variables inserted in order to find a initial feasible solution for the problem
		 */
		static bool artificialMinStep(Table& instance, int* rowBasicData);
		
		static double findRatio(Table& instance, int row, int column, int resCol);
		static void makeRowUnit(Table& instance, int row, int col);
		static void makeOtherRowsUnit(Table& instance, int baseRow, int col);
		static void setupArtificialTable(Table& instance, Table& original, std::vector<int> const& artificialVariables);
		static void restoreTable(Table& instance, Table& original);
	public:
		/**
		 * Solve the simplex tableau
		 * NOTE: It is assumed that the last column in the table is the results column
		 */
		static bool solveTable(Table& instance, SimplexResult& results);
		
		/**
		 * Set the solvers logging level
		 */
		static void setLogging(bool enabled);
	};
}

#endif //_TABLEAU_SOLVER_DEF_H_
