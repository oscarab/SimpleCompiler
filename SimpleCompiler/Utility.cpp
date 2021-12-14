#include <unordered_map>
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include "Token.h"

extern const char* TokenTypeStr[] = { "KEY_WORD", "ID", "OPERATOR", "CONSTANT",
							"BOUNDARY", "BRACKET" , "END"};
extern const char* TokenAttrStr[] = { "int", "float", "void", "if", "else", "while",
							"return", "+", "-", "*", "/", "=", "==",
							">", "<", ">=", "<=", "!=", "&&", "||", ",", ";",
							"{", "}", "(", ")", "CONSTANT", "ID", "NONE"};

extern const std::unordered_map<std::string, Token> tokenConvert{
	{"int", Token(TokenType::KEY_WORD, TokenAttribute::_int)},
	{"float", Token(TokenType::KEY_WORD, TokenAttribute::_float)},
	{"void", Token(TokenType::KEY_WORD, TokenAttribute::_void)},
	{"if", Token(TokenType::KEY_WORD, TokenAttribute::_if)},
	{"else", Token(TokenType::KEY_WORD, TokenAttribute::_else)},
	{"while", Token(TokenType::KEY_WORD, TokenAttribute::_while)},
	{"return", Token(TokenType::KEY_WORD, TokenAttribute::_return)},
	{"id", Token(TokenType::ID, TokenAttribute::RealID)},
	{"num", Token(TokenType::CONSTANT, TokenAttribute::RealConstant)},
	{"=", Token(TokenType::OPERATOR, TokenAttribute::Assign)},
	{"+", Token(TokenType::OPERATOR, TokenAttribute::Add)},
	{"-", Token(TokenType::OPERATOR, TokenAttribute::Minus)},
	{"*", Token(TokenType::OPERATOR, TokenAttribute::Multiply)},
	{"/", Token(TokenType::OPERATOR, TokenAttribute::Divide)},
	{"<", Token(TokenType::OPERATOR, TokenAttribute::Less)},
	{">", Token(TokenType::OPERATOR, TokenAttribute::Greater)},
	{"<=", Token(TokenType::OPERATOR, TokenAttribute::Lequal)},
	{">=", Token(TokenType::OPERATOR, TokenAttribute::Gequal)},
	{"==", Token(TokenType::OPERATOR, TokenAttribute::Equal)},
	{"!=", Token(TokenType::OPERATOR, TokenAttribute::Nequal)},
	{"&&", Token(TokenType::OPERATOR, TokenAttribute::AND)},
	{"||", Token(TokenType::OPERATOR, TokenAttribute::OR)},
	{",", Token(TokenType::BOUNDARY, TokenAttribute::Comma)},
	{";", Token(TokenType::BOUNDARY, TokenAttribute::Semicolon)},
	{"{", Token(TokenType::BRACKET, TokenAttribute::LeftBrace)},
	{"}", Token(TokenType::BRACKET, TokenAttribute::RightBrace)},
	{"(", Token(TokenType::BRACKET, TokenAttribute::LeftBracket)},
	{")", Token(TokenType::BRACKET, TokenAttribute::RightBracket)},
	{"#", Token(TokenType::END, TokenAttribute::None)},
	{"空", Token(TokenType::EPSILON, TokenAttribute::None)}
};

std::vector<std::string> idTable;	// 词法分析标识符表
std::vector<double> constantTable;	// 词法分析常数表

/**
 * @brief 输出TAB
 * @param out 输出源
 * @param level 数量
*/
void tab(std::ostream& out, int level) {
	while (level--) {
		out << '\t';
	}
}

int getWidth(int number, std::string str) {
	int width = 0;

	while (number > 0) {
		number /= 10;
		width++;
	}

	return std::max(width, int(str.size()));
}

std::string convertToString(Token& token) {
	if (token.getType() == TokenType::ID && token.getIndex() >= 0) {
		return idTable[token.getIndex()];
	}
	else if (token.getType() == TokenType::CONSTANT && token.getIndex() >= 0){
		return std::to_string(constantTable[token.getIndex()]);
	}
	else {
		if (token.getType() == TokenType::END) {
			return std::string(TokenTypeStr[int(token.getType())]);
		}
		else {
			return std::string(TokenAttrStr[int(token.getAttribute())]);
		}
	}
}