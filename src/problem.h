#ifndef _PROBLEM_DEF_H_
#define _PROBLEM_DEF_H_
#include "constraint.h"
#include "tableau.h"

namespace Constraints {
	class Problem {
	private:
		std::vector<Variable> _variables;
		std::vector<Constraint> _constraints;
		unsigned int _lastVariableId;
		
		inline bool isSolvable(Simplex::Table& currentTable, unsigned int i) const;
		bool simSat(Simplex::Table& currentTable, unsigned int i) const;
	public:
		Problem();
		~Problem();

		/**
		 * Create a new variable within the problem
		 */
		Variable createVariable(std::string const& name);
		
		/**
		 * Add a new constraint to this problem
		 */
		void addConstraint(Constraint const& constraint);

		/**
		 * Check if there is an allocation which satisfies this problem
		 */
		bool isSatisfiable() const;
		
		/**
		 * Serialize information about this problem to a string
		 */
		std::string toString() const;
	};
}

#endif //_PROBLEM_DEF_H_
