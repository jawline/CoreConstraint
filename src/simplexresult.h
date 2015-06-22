#ifndef _SIMPLEX_RESULT_DEF_H_
#define _SIMPLEX_RESULT_DEF_H_
#include "tableau.h"
#include <map>

namespace Simplex {

    /**
     * Structure to hold results of simplex solver run
     */
    class SimplexResult {
    private:
        std::map<int, double> _results;
    public:
        SimplexResult();
        SimplexResult(Table const& instance, int* rowBasis);
        ~SimplexResult();
        
        double getResult() const;
        bool getResult(int column, double& result) const;
    };
};

#endif //_SIMPLEX_RESULT_DEF_H_
