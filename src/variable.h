#ifndef _VARIABLE_DEF_H_
#define _VARIABLE_DEF_H_
#include <string>

namespace Constraints {
	class Variable {
	private:
		std::string _name;
		bool _isSigned;
		unsigned int _variableIndex;
		
	public:
		Variable();
		Variable(std::string const& name, bool isSigned, unsigned int index);
		~Variable();
		
		/**
		 * Return the variables name
		 */
		std::string toString() const;
		
		/**
		 * Return true if this is a signed integer
		 */
		bool isSigned() const;
		
		/**
		 * Get this variables index in the problem
		 */
		unsigned int getVariableIndex() const;

		/**
		 * Returns true if its a reference to the same variable
		 */
		bool equals(Variable const& other) const;
	};
}

#endif //_VARIABLE_DEF_H_
