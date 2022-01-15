#include "Lexer/Lexer.h"

#include <sstream>

// 指针前移并判断是否需要请求新数据
#define STEP_OR_REQUEST {if (step() == 0) { \
						rollback();\
						return Token(TokenType::INCOMPLETE, TokenAttribute::None);}}

using namespace Lexical;

extern std::vector<std::string> idTable;
extern std::vector<double> constantTable;
/* 扫描器 */

// 构造函数
Scanner::Scanner() {
	endPoint = 0;
	isComplete = 0;

	startPoint = -1;
	scanPoint = -1;

	buffer = NULL;
}

// 获取buffer数组
void Scanner::setBuffer(char* buffer) {
	this->buffer = buffer;
}

// 获取结束位置
void Scanner::setEndPoint(int endPoint) {
	this->endPoint = endPoint;
}

// 获取完整性
void Scanner::setIsComplete(int isComplete) {
	this->isComplete = isComplete;
}

// 判断数字
bool Scanner::isDigit() {
	return (buffer[scanPoint] >= '0' && buffer[scanPoint] <= '9');
}

// 判断字母
bool Scanner::isLetter() {
	return ((buffer[scanPoint] >= 'a' && buffer[scanPoint] <= 'z') || (buffer[scanPoint] >= 'A' && buffer[scanPoint] <= 'Z'));
}

// 是否已经扫描完成
int Scanner::isReachComplete() {
	if (scanPoint >= endPoint) {
		return isComplete;
	}
	return -1;
}

// 指针前进
int Scanner::step() {
	if (scanPoint + 1 >= endPoint) {
		scanPoint = (scanPoint + 1) % (2 * BLOCK_SIZE);
		return isComplete;
	}
	scanPoint = (scanPoint + 1) % (2 *BLOCK_SIZE);
	return isReachComplete();
}

// 指针后退
void Scanner::retract() {
	scanPoint--;
}

// 指针对齐
void Scanner::align() {
	startPoint = ++scanPoint;
}

// 撤销已读入的字符
void Scanner::rollback() {
	scanPoint = --startPoint;
}

// 跳过空白
void Scanner::skipBlank() {
	while (buffer[scanPoint] == ' ') {
		startPoint++;
		scanPoint++;
	}
}

// 查找关键字表
TokenAttribute Scanner::reserve() {
	std::string str = std::string(buffer + startPoint, (scanPoint - startPoint) + 1);

	if (str == "int")
		return TokenAttribute::_int;
	else if (str == "float")
		return TokenAttribute::_float;
	else if (str == "void")
		return TokenAttribute::_void;
	else if (str == "if")
		return TokenAttribute::_if;
	else if (str == "else")
		return TokenAttribute::_else;
	else if (str == "while")
		return TokenAttribute::_while;
	else if (str == "return")
		return TokenAttribute::_return;
	else
		return TokenAttribute::RealID;
}

// 加入新标识符
int Scanner::insertID() {
	std::string str = std::string(buffer + startPoint, (scanPoint - startPoint) + 1);
	int index;

	if (tablePoint.find(str) == tablePoint.end()) {
		idTable.push_back(str);	// 插入标识符表
		index = idTable.size() - 1;
		tablePoint[str] = index;
	}
	else {
		index = tablePoint[str];
	}
	
	return index;
}

// 加入新常数
int Scanner::insertConstant() {
	std::string str = std::string(buffer + startPoint, (scanPoint - startPoint) + 1);
	int index;
	double constant;
	std::stringstream strstream;

	// 字符串转实数
	strstream << str;
	strstream >> constant;

	if (tablePoint.find(str) == tablePoint.end()) {
		// 插入常数表
		constantTable.push_back(constant);
		index = constantTable.size() - 1;
		tablePoint[str] = index;
	}
	else {
		index = tablePoint[str];
	}
	
	return index;
}

// 扫描
Token Scanner::scan() {
	int value;					// 索引值
	TokenAttribute attr;		// 关键字
	align();					// 统一指针位置
	skipBlank();				// 跳过空白

	if (isReachComplete() == 1) {
		// 已经完成扫描
		return Token(TokenType::COMPLETE, TokenAttribute::None);
	}
	else if (isReachComplete() == 0) {
		// 可能还有后续数据，请求数据
		startPoint = scanPoint = endPoint % (2 * BLOCK_SIZE) - 1;
		return Token(TokenType::INCOMPLETE, TokenAttribute::None);
	}

	// 遇到字母
	if (isLetter()) {
		while (isLetter() || isDigit()) {
			STEP_OR_REQUEST;
		}
		retract();
		attr = reserve();
		if (attr == TokenAttribute::RealID) {
			// 自定义标识符
			value = insertID();
			return Token(TokenType::ID, TokenAttribute::RealID, value);
		}
		else {
			// 关键字
			return Token(TokenType::KEY_WORD, attr);
		}
	}
	else if (isDigit()) {	// 遇到数字
		while (isDigit()) {
			// 读取整数部分
			STEP_OR_REQUEST;
		}
		if (buffer[scanPoint] == '.') {
			STEP_OR_REQUEST;
			while (isDigit()) {
				// 读取小数部分
				STEP_OR_REQUEST;
			}
			if (buffer[scanPoint] == 'e' || buffer[scanPoint] == 'E') {
				STEP_OR_REQUEST;
				if (buffer[scanPoint] == '-') STEP_OR_REQUEST;
				if (!isDigit()) return Token(TokenType::FAIL, TokenAttribute::None);
				while (isDigit()) {
					// 读取指数部分
					STEP_OR_REQUEST;
				}
			}
		}
		retract();
		value = insertConstant();
		return Token(TokenType::CONSTANT, TokenAttribute::RealConstant, value);
	}
	else if (buffer[scanPoint] == '+') return Token(TokenType::OPERATOR, TokenAttribute::Add);
	else if (buffer[scanPoint] == '-') return Token(TokenType::OPERATOR, TokenAttribute::Minus);
	else if (buffer[scanPoint] == '*') return Token(TokenType::OPERATOR, TokenAttribute::Multiply);
	else if (buffer[scanPoint] == '/') return Token(TokenType::OPERATOR, TokenAttribute::Divide);
	else if (buffer[scanPoint] == '=') {
		STEP_OR_REQUEST;
		if (buffer[scanPoint] == '=') {
			return Token(TokenType::OPERATOR, TokenAttribute::Equal);
		}
		retract();
		return Token(TokenType::OPERATOR, TokenAttribute::Assign);
	}
	else if (buffer[scanPoint] == '>') {
		STEP_OR_REQUEST;
		if (buffer[scanPoint] == '=') return Token(TokenType::OPERATOR, TokenAttribute::Gequal);	// >=
		retract();
		return Token(TokenType::OPERATOR, TokenAttribute::Greater);
	}
	else if (buffer[scanPoint] == '<') {
		STEP_OR_REQUEST;
		if (buffer[scanPoint] == '=') return Token(TokenType::OPERATOR, TokenAttribute::Lequal);	// <=
		retract();
		return Token(TokenType::OPERATOR, TokenAttribute::Less);
	}
	else if (buffer[scanPoint] == '!') {
		STEP_OR_REQUEST;
		if (buffer[scanPoint] == '=') {
			return Token(TokenType::OPERATOR, TokenAttribute::Nequal);
		}
		else {
			return Token(TokenType::FAIL, TokenAttribute::None);
		}
	}
	else if (buffer[scanPoint] == '&') {
		STEP_OR_REQUEST;
		if (buffer[scanPoint] == '&') {
			return Token(TokenType::OPERATOR, TokenAttribute::AND);
		}
		else {
			return Token(TokenType::FAIL, TokenAttribute::None);
		}
	}
	else if (buffer[scanPoint] == '|') {
		STEP_OR_REQUEST;
		if (buffer[scanPoint] == '|') {
			return Token(TokenType::OPERATOR, TokenAttribute::OR);
		}
		else {
			return Token(TokenType::FAIL, TokenAttribute::OR);
		}
	}
	else if (buffer[scanPoint] == ',') return Token(TokenType::BOUNDARY, TokenAttribute::Comma);
	else if (buffer[scanPoint] == ';') return Token(TokenType::BOUNDARY, TokenAttribute::Semicolon);
	else if (buffer[scanPoint] == '{') return Token(TokenType::BRACKET, TokenAttribute::LeftBrace);
	else if (buffer[scanPoint] == '}') return Token(TokenType::BRACKET, TokenAttribute::RightBrace);
	else if (buffer[scanPoint] == '(') return Token(TokenType::BRACKET, TokenAttribute::LeftBracket);
	else if (buffer[scanPoint] == ')') return Token(TokenType::BRACKET, TokenAttribute::RightBracket);
	else {
		// 错误处理
		return Token(TokenType::FAIL, TokenAttribute::None);
	}

}