#include "simplexresult.h"
using namespace Simplex;

SimplexResult::SimplexResult() {}

SimplexResult::SimplexResult(Table const& instance, int* rowBasis) {
  
  //Results column result is always 0,0
  _results[0] = instance.getField(0,0);
  
  //Map the results column to the result basis's
  for (unsigned int i = 1; i < instance.getNumRows(); i++) {
    _results[rowBasis[i]] = instance.getField(i, 0);
  }
}

SimplexResult::~SimplexResult() {}

double SimplexResult::getResult() const {
  return _results.find(0)->second;
}

bool SimplexResult::getResult(int column, double& result) const {
  if (_results.find(column) != _results.end()) {
    result = _results.find(column)->second;
    return true;
  }
  return false;
}
