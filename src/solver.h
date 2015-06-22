#ifndef _TABLEAU_SOLVER_DEF_H_
#define _TABLEAU_SOLVER_DEF_H_
#include <vector>
#include "tableau.h"
#include "simplexresult.h"

namespace Simplex {

	class Solver {
	private:
		static bool _excessiveLogging;
		static unsigned int _lastArtificial;
		static int findBasicRow(Table& instance, int col);
		static int findBasic(Table& instance, int row);
		static int findPivotColumn(Table& instance, bool minimize);
		static double findRatio(Table& instance, int row, int column, int resCol);
		static int findPivotRow(Table& instance, int column);
		static void makeRowUnit(Table& instance, int row, int col);
		static void makeOtherRowsUnit(Table& instance, int baseRow, int col);
		static void setupArtificialTable(Table& instance, Table& original, std::vector<int> const& artificialVariables);
		static void restoreTable(Table& instance, Table& original);
		static void findBasicData(Table& instance, int* rowBasicData);
		static bool artificialMinStep(Table& instance, int* rowBasicData);
		static void handleFinalBasicData(Table& instance, int* rowBasicData);
		static bool pivotTable(Table& instance, int* rowBasicData, bool minimize);
		static void doPivot(Table& instance, int* basis, unsigned int row, unsigned int col);
		static bool allArtificialsZero(Table const& instance, std::vector<int> const& artificialVariables);
		static bool artificialColumnsInBasis(int* basis, unsigned int numRows, std::vector<int> const& artificialColumns);

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
