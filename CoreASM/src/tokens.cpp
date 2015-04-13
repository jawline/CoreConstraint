#include "tokens.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

extern "C" {
	#include <regexpm/match.h>
}

using namespace Assembler;

Token::Token(TOKEN_ID id) {
	_id = id;
	_data = "";
}

Token::Token(TOKEN_ID id, char const* input, size_t len) {
	_id = id;
	_data = std::string(input, len);
}

Token::~Token() {}

TOKEN_ID Token::tokenId() const {
	return _id;
}

char const* Token::tokenString() const {
	return _data.c_str();
}

Tokeniser::Tokeniser() {
	regexParse(&idRegex, "[a-zA-Z][a-zA-Z0-9]*");
	regexParse(&intRegex, "[0-9]+");
}

Tokeniser::~Tokeniser() {
	regexFree(&idRegex);
	regexFree(&intRegex);
}

char const* Tokeniser::skipWhite(char const* input) {
  for (; *input && isspace(*input); input++) {}
  return input;
}

Token Tokeniser::nextToken(char const*& input) {
	size_t len;
	Token t = peekToken(input, len);
	input += len;
}

Token Tokeniser::peekToken(char const* input) {
	size_t len;
	return peekToken(input, len);
}

Token Tokeniser::peekToken(char const* input, size_t& len) {
	Token result = Token(INVALID_TOKEN);
	input = skipWhite(input);

	if (*input == '\0') {
		result = Token(TOKEN_EOF);
		len = 0;
	} else if (*input == ':') {
		result = Token(COLON, input, 1);
		len = 1;
	} else if (strncmp(input, "jmp", 3) == 0) {
		result = Token(JUMP, input, 3);
		len = 3;
	} else if (strncmp(input, "load", 3) == 0) {
		result = Token(LOAD, input, 4);
		len = 4;
	} else if ((len = nfaMatches(idRegex.start, input)) > 0) {
		result = Token(ID, input, regexLen);
	} else if ((len = nfaMatches(intRegex.start, input)) > 0) {
		result = Token(NUM, input, regexLen);
	}

	return result;
}
