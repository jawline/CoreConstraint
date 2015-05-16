#ifndef _ADD_VALUE_SYMBOL_DEF_H_
#define _ADD_VALUE_SYMBOL_DEF_H_
#include "compositesymbol.h"

namespace VM {
  class AddValueSymbol: public CompositeSymbol {
  private:
    CompositeSymbol* _symbol;
    //TODO: Pick a type that is equal to a registers size
    int _value;
  protected:
  	void addToConstraint(Constraints::Constraint& constraint);
  public:
    AddValueSymbol(CompositeSymbol* symbol, int value);
    virtual ~AddValueSymbol();
  };
}

#endif //_ADD_VALUE_SYMBOL_DEF_H_
