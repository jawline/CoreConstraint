#include "parser.h"
#include "problem_constants.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

using namespace Constraints;

#define CHECKINPUT() if (!input) { return nullptr; }
#define CHECKINPUTB() if (!input) { return false; }

extern "C" {
  #include <regexpm/match.h>
  #include <regexpm/parser.h>
}

regex idRegex;
regex numRegex;

/**
 * Skip whitespace until the end of the input or the next character is found
 */
char const* skipWhite(char const* input) {
  for (; *input && isspace(*input); input++) {}
  return input;
}

/**
 * Get the next token from the input string
 */
char const* nextToken(TOKEN* token, char const* input, char const** tokenStart, size_t* tokenSize) {
  size_t regexLen;
  input = skipWhite(input);
  *tokenStart = input;

  if (strncmp(input, "max", 3) == 0) {
    *token = MAX;
    *tokenSize = 3;
    return input + 3;
  } else if (strncmp(input, "s.t.", 4) == 0) {
    *token = ST;
    *tokenSize = 4;
    return input + 4;
  } else if (*input == ',') {
    *token = COMMA;
    *tokenSize = 1;
    return input + 1;
  } else if (strncmp(input, "!=", 2) == 0) {
    *token = NE;
    *tokenSize = 2;
    return input + 2;
  } else if (*input == '=') {
    *token = EQ;
    *tokenSize = 1;
    return input + 1;
  } else if (*input == '+') {
    *token = PLUS;
    *tokenSize = 1;
    return input + 1;
  } else if (*input == '-') {
    *token = MINUS;
    *tokenSize = 1;
    return input + 1;
  } else if (strncmp(input, "<=", 2) == 0) {
    *token = LTOE;
    *tokenSize = 2;
    return input + 2;
  } else if (strncmp(input, ">=", 2) == 0) {
    *token = GTOE;
    *tokenSize = 2;
    return input + 2;
  } else if (*input == '<') {
    *token = GT;
    *tokenSize = 1;
    return input + 1;
  } else if (*input == '>') {
    *token = GT;
    *tokenSize = 1;
    return input + 1;
  } else if (*input == '\0') {
    *token = PEOF;
    *tokenSize = 0;
    return input;
  } else if ((regexLen = nfaMatches(idRegex.start, input)) > 0) {
    *token = ID;
    *tokenSize = regexLen;
    return input + regexLen;
  } else if ((regexLen = nfaMatches(numRegex.start, input)) > 0) {
    *token = NUM;
    *tokenSize = regexLen;
    return input + regexLen;
  }
  
  printf("Could not parse valid token from \"%s\"\n", input);
  return 0;
}

void parserInit() {
  regexParse(&idRegex, "[a-zA-Z][a-zA-Z0-9]*");
  regexParse(&numRegex, "[0-9]+(.[0-9]+)?");
}

char const* parseExpression(Problem& instance, Constraint& constraint, char const* input, bool objective, int scale) {
  TOKEN token;
  size_t tokenSize;
  char const* tempInput;
  char const* tokenStart;

  input = nextToken(&token, input, &tokenStart, &tokenSize);

  CHECKINPUT();

  if (token == MINUS) {
    input = nextToken(&token, input, &tokenStart, &tokenSize);
    scale *= -1;
  }

  CHECKINPUT();

  int scalar = objective ? -1 : 1;
  scalar *= scale;
  
  Variable var = instance.getVariableWithName(std::string(tokenStart, tokenSize));
  
  if (token == ID) {
    constraint.addItem(var, scalar);
  } else if (token == NUM) {
    double parsedValueAsNumber;
    
    if (!sscanf(tokenStart, "%lf", &parsedValueAsNumber)) {
      printf("Unable to parse number from NUM token\n");
      return 0;
    }

    if ((tempInput = nextToken(&token, input, &tokenStart, &tokenSize)) && token == ID) {
      constraint.addItem(var, parsedValueAsNumber * scalar);
      input = tempInput;
    } else {
      printf("Expected ID or NUM ID near \"%s\"", input);
      return 0;
    }
  } else {
    printf("Expected ID, NUM or NUM ID near \"%s\"\n", input);
    return 0;
  }
  
  if ((tempInput = nextToken(&token, input, &tokenStart, &tokenSize)) && (token == PLUS || token == MINUS)) {
    return parseExpression(instance, constraint, tempInput, objective, token == MINUS ? -1 : 1);
  } else {
    return input;
  }
}

char const* parseConstraint(Problem& instance, char const* input) {
  TOKEN token;
  size_t tokenSize;
  char const* tokenStart;

  Constraint constraint;
  
  input = parseExpression(instance, constraint, input, false, 1);

  CHECKINPUT();
  
  input = nextToken(&token, input, &tokenStart, &tokenSize);
  
  CHECKINPUT();
  
  if (token == EQ) {
    constraint.setComparisonType(Equal);
  } else if (token == NE) {
    constraint.setComparisonType(NotEqual);
  } else if (token == GT) {
    constraint.setComparisonType(GreaterThan);
  } else if (token == LT) {
    constraint.setComparisonType(LessThan);
  } else if (token == LTOE) {
    constraint.setComparisonType(LessThanOrEqual);
  } else if (token == GTOE) {
    constraint.setComparisonType(GreaterThanOrEqual);
  } else {
    printf("Expected equals near %s\n", input);
    return 0;
  }
  
  input = nextToken(&token, input, &tokenStart, &tokenSize);
  
  CHECKINPUT();
  
  if (token != NUM) {
    printf("Expected NUM near %s\n", input);
    return 0;
  }

  double parsedValueAsNumber;

  if (!sscanf(tokenStart, "%lf", &parsedValueAsNumber)) {
    printf("SSCANF FAIL\n");
    return 0;
  }

  constraint.setResult(parsedValueAsNumber);
  instance.addConstraint(constraint);
  return input;
}

char const* parseConstraints(Problem& instance, char const* input) {
  TOKEN token;
  size_t tokenSize;
  char const* tempInput;
  char const* tokenStart;
  input = parseConstraint(instance, input);
  
  CHECKINPUT();
  
  if ((tempInput = nextToken(&token, input, &tokenStart, &tokenSize)) && token == COMMA) {
    return parseConstraints(instance, tempInput);
  } else {
    return input;
  }
}

bool postParseStep(Problem& instance) {
  return true;
}

bool parseString(Problem& instance, char const* input) {
  TOKEN token;
  char const* tokenStart;
  size_t tokenSize;

  input = parseConstraints(instance, input);
  CHECKINPUTB();
  
  input = nextToken(&token, input, &tokenStart, &tokenSize);
  CHECKINPUTB();
  
  if (token != PEOF) {
    printf("Unexpected symbol near \"%s\".\n", input);
    return false;
  } else {
    return postParseStep(instance);
  }
}

void parserFree() {
  regexFree(&idRegex);
  regexFree(&numRegex);
}
