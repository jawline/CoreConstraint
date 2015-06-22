#ifndef _SOLVER_CONSTRAINT_DEF_H_
#define _SOLVER_CONSTRAINT_DEF_H_
#include "variable.h"
#include "tableau.h"
#include <vector>
#include <string>

namespace Constraints {

	typedef std::pair<Variable,double> ConstraintItem;

	/**
	 * The different types of comparison in a constraint
	 */
	typedef enum {
		Invalid = 0,
		Equal,
		NotEqual,
		LessThan,
		GreaterThan,
		GreaterThanOrEqual,
		LessThanOrEqual,
		NumComparisonTypes
	} ComparisonType;

	class Constraint {
	private:
		static const char* ComparisonTypeStrings[];
		std::vector<ConstraintItem> _items;
		ComparisonType _type;
		double _value;
	public:
		Constraint();
		Constraint(double value);
		~Constraint();

		/**
		 * Add a new variable to the constraint
		 */
		void addItem(Variable const& var, double multiplier);
		
		/**
		 * Set the result to be compared against
		 */
		void setResult(double endValue);
		void buildResult(ComparisonType type, double comparisonValue);

		/**
		 * Get the constraint result
		 */
		double getResult() const;
		
		/**
		 * Set the comparison type on this constraint
		 */
		void setComparisonType(ComparisonType const& type);
		
		/**
		 * Get the comparison type of this constraint
		 */
		ComparisonType getComparisonType() const;

		/**
		 * Return a human readable description of the constraint
		 */
		std::string toString() const;
		
		/**
		 * Add this constraint to a simplex table, if typeOverride is anything other than
		 * invalid then it will be used instead of the constraints
		 */
		void addToTable(Simplex::Table& table, ComparisonType typeOverride = ComparisonType::Invalid) const;

		/**
		 * Combine this and another constraint
		 */
		Constraint& add(Constraint const& other);
		Constraint& minus(Constraint const& other);
		Constraint& multiply(Constraint const& other);
		Constraint& divide(Constraint const& other);

		/**
		 * Scale all values in this constraint
		 */
		Constraint& scale(double scalar);
	};
};

#endif //_SOLVER_CONSTRAINT_DEF_H_
