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
        
        /**
         * Get the final result calculated by the simplex solver
         */
        double getResult() const;
        
        /**
         * Get the result of the specified column
         */
        bool getResult(int column, double& result) const;
    };
};

#endif //_SIMPLEX_RESULT_DEF_H_
