#ifndef EXCEPTION

#include <exception>
#include <string>
#include "Lexer/Token.h"

using std::string;

class CompilerException : public std::exception {

private:
	string error;
	Token token;

public:
	CompilerException(string message, Token& t) : token(t) {
		error = message;
	}

	CompilerException(string message) : token(TokenType::FAIL, TokenAttribute::None) {
		error = message;
	}

	string getError() const {
		return error;
	}

	Token getToken() const {
		return token;
	}

	const char* what() const throw () {
		return error.c_str();
	}
};

#endif // !EXCEPTION
