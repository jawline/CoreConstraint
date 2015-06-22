#include "parser.h"
#include "problem_constants.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

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

char const* parseExpression(Table& instance, char const* input, bool objective, int scale) {
  TOKEN token;
  size_t tokenSize;
  char const* tempInput;
  char const* tokenStart;

  input = nextToken(&token, input, &tokenStart, &tokenSize);

  if (!input) {
    return 0;
  }

  if (token == MINUS) {
    input = nextToken(&token, input, &tokenStart, &tokenSize);
    scale *= -1;
  }

  if (!input) {
    return 0;
  }

  int scalar = objective ? -1 : 1;
  scalar *= scale;
  
  if (token == ID) {
    instance.addColumn(std::string(tokenStart, tokenSize));
    instance.setField(instance.getCurrentRow(), std::string(tokenStart, tokenSize), scalar);
  } else if (token == NUM) {
    double parsedValueAsNumber;
    
    if (!sscanf(tokenStart, "%lf", &parsedValueAsNumber)) {
      printf("Unable to parse number from NUM token\n");
      return 0;
    }

    if ((tempInput = nextToken(&token, input, &tokenStart, &tokenSize)) && token == ID) {
      instance.addColumn(std::string(tokenStart, tokenSize));
      instance.setField(instance.getCurrentRow(), std::string(tokenStart, tokenSize), parsedValueAsNumber * scalar);
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
    return parseExpression(instance, tempInput, objective, token == MINUS ? -1 : 1);
  } else {
    return input;
  }
}

char const* parseConstraint(Table& instance, char const* input) {
  TOKEN token;
  size_t tokenSize;
  char const* tokenStart;

  instance.addRow();
  
  input = parseExpression(instance, input, false, 1);

  if (!input) {
    return 0;
  }
  
  input = nextToken(&token, input, &tokenStart, &tokenSize);
  
  if (!input) {
    return 0;
  }
  
  if (token != EQ) {
    printf("Expected equals near %s\n", input);
    return 0;
  }
  
  input = nextToken(&token, input, &tokenStart, &tokenSize);
  
  if (!input) {
    return 0;
  }
  
  if (token != NUM) {
    printf("Expected NUM near %s\n", input);
    return 0;
  }

  double parsedValueAsNumber;
  if (!sscanf(tokenStart, "%lf", &parsedValueAsNumber)) {
    printf("SSCANF FAIL\n");
    return 0;
  }

  instance.setField(instance.getCurrentRow(), Constraints::ProblemConstants::cResultColumnName, parsedValueAsNumber);
  return input;
}

char const* parseConstraints(Table& instance, char const* input) {
  TOKEN token;
  size_t tokenSize;
  char const* tempInput;
  char const* tokenStart;
  input = parseConstraint(instance, input);
  
  if (!input) {
    return 0;
  }
  
  if ((tempInput = nextToken(&token, input, &tokenStart, &tokenSize)) && token == COMMA) {
    return parseConstraints(instance, tempInput);
  } else {
    return input;
  }
}

bool postParseStep(Table& instance) {
  return true;
}

bool parseString(Table& instance, char const* input) {
  TOKEN token;
  char const* tokenStart;
  size_t tokenSize;
  
  instance.addColumn(Constraints::ProblemConstants::cResultColumnName);
  instance.addRow();
  
  input = nextToken(&token, input, &tokenStart, &tokenSize);
  
  if (!input) {
    return false;
  }

  if (token != MAX) {
    printf("Expected max to be the next token near %s\n", input);
    return false;
  }
  
  input = nextToken(&token, input, &tokenStart, &tokenSize);
  
  if (!input) {
    return false;
  }
  
  if (token != ID) {
    printf("Expected ID near \"%s\"", input);
    return false;
  }

  instance.addColumn(std::string(tokenStart, tokenSize));
  instance.setField(instance.getCurrentRow(), std::string(tokenStart, tokenSize), 1);
  
  input = nextToken(&token, input, &tokenStart, &tokenSize);
  
  if (!input) {
    return false;
  }
  
  if (token != EQ) {
    printf("Expected EQ near \"%s\"", input);
    return false;
  }
  
  input = parseExpression(instance, input, true, 1);
  
  if (!input) {
    return false;
  }
  
  input = nextToken(&token, input, &tokenStart, &tokenSize);
  
  if (!input) {
    return false;
  }
  
  if (token == PEOF) {
    return postParseStep(instance);
  } else if (token != ST) {
    printf("Expected 's.t.' to be the next token near \"%s\"\n", input);
    return false;
  }
  
  input = parseConstraints(instance, input);
  
  if (!input) {
    return false;
  }
  
  input = nextToken(&token, input, &tokenStart, &tokenSize);
  
  if (!input) {
    return false;
  }
  
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
