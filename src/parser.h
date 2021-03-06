#ifndef _SIMPLEX_PARSER_DEF_H_
#define _SIMPLEX_PARSER_DEF_H_
#include <stdbool.h>
#include "tableau.h"
#include "problem.h"

using namespace Simplex;

/**
 * ID = \[a-zA-Z][a-zA-Z0-9]*\
 * NUM = \[0-9]+(.[0-9]+)?\
 * Grammar:
 * MinMax: max ID EQ Expression ['s.t.' Constraints]
 * Constraints: Constraint [',' Constraints]
 * Constraint: Expression '=' NUM
 * Expression: (ID | NUM ID) [('+' | '-') Expression]
 */
 
typedef enum {
 MAX = 0,
 ID,
 NUM,
 ST,
 EQ,
 NE,
 GT,
 LT,
 GTOE,
 LTOE,
 PLUS,
 MINUS,
 COMMA,
 PEOF
} TOKEN;
 
 void parserInit();
 bool parseString(Constraints::Problem& instance, char const* input);
 void parserFree();

#endif //_SIMPLEX_PARSER_DEF_H_
