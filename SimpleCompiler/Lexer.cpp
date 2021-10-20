#include "Lexer.h"
//#include <unordered_set>

using namespace LexicalAnalysis;

Lexer::Lexer(char* fileName) {
	idTable = std::vector<std::string>(126);
	constantTable = std::vector<double>(126);

	codeReader = Reader(fileName);
	scanner = Scanner(&idTable, &constantTable);
}

void Lexer::run() {
	
}



/* 扫描器 */

// 构造函数
Scanner::Scanner(std::vector<std::string>* id_table, std::vector<double>* constant_table) {
	idTable = id_table;
	constantTable = constant_table;

	endPoint = 0;
	isComplete = 0;

	startPoint = -1;
	scanPoint = -1;
	// 哈希表
	//std::unordered_set<std::string> hashsetTokenType;
	//std::unordered_set<std::string> hashsetTokenAttribute;

}

// 获取buffer数组
void Scanner::setBuffer(char* buffer) {
	this->buffer = buffer;
}

// 获取结束位置
void Scanner::setEndPoint(int endPoint) {
	this->endPoint = endPoint;
}

// 获取完整度
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

// 指针前进
void Scanner::step() {
	scanPoint++;
}

// 指针后退
void Scanner::retract() {
	//if(!startPoint)
	scanPoint--;
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
	std::string str = buffer;
	str = str.substr(startPoint + 1, (scanPoint - startPoint));
	if (str == "int")
		return TokenAttribute::_int;
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
		return TokenAttribute::_others;
}

// 加入新标识符
int Scanner::insertID() {
	std::string str = buffer;
	str = str.substr(startPoint + 1, (scanPoint - startPoint));
	idTable->push_back(str);	// 插入标识符表
	return idTable->size() - 1;
}

// 加入新常数
int Scanner::insertConstant() {
	std::string str = buffer;
	str = str.substr(startPoint + 1, (scanPoint - startPoint));
	double constant = atof(str.c_str());
	constantTable->push_back(constant);	// 插入常数表
	return constantTable->size() - 1;
}

// 组装Token
//Token Scanner::createToken() {
//}

// 扫描
Token Scanner::scan() {
	int value;	// 索引值
	TokenAttribute attr;	// 关键字
	startPoint = scanPoint;	// 统一指针位置

	step();	// 前进，扫描一个字符
	if (scanPoint == endPoint) {
		if(isComplete)
			return Token(TokenType::COMPLETE, TokenAttribute::_complete);
		else
			return Token(TokenType::INCOMPLETE, TokenAttribute::_incomplete);
	}
	skipBlank();	// 跳过空白

	// 遇到字母
	if (isLetter()) {
		while (isLetter() || isDigit())
			step();
		retract();
		attr = reserve();
		if (attr == TokenAttribute::_others) {	// 自定义标识符
			value = insertID();
			return Token(TokenType::ID, TokenAttribute::RealString, value);
		}
		else // 关键字
			return Token(TokenType::KEY_WORD, attr);
	}
	else if (isDigit()) {	// 遇到数字
		while (isDigit())
			step(); // 小数待补充
		retract();
		value = insertConstant();
		return Token(TokenType::CONSTANT, TokenAttribute::RealConstant, value);
	}
	else if (buffer[scanPoint] == '+') return Token(TokenType::OPERATOR, TokenAttribute::Add);
	else if (buffer[scanPoint] == '-') return Token(TokenType::OPERATOR, TokenAttribute::Minus);
	else if (buffer[scanPoint] == '*') return Token(TokenType::OPERATOR, TokenAttribute::Multiply);
	else if (buffer[scanPoint] == '/') return Token(TokenType::OPERATOR, TokenAttribute::Divide);
	else if (buffer[scanPoint] == '=') {
		step();
		if (buffer[scanPoint] == '=') return Token(TokenType::OPERATOR, TokenAttribute::Equal);		// ==
		retract();
		return Token(TokenType::OPERATOR, TokenAttribute::Assign);
	}
	else if (buffer[scanPoint] == '>') {
		step();
		if (buffer[scanPoint] == '=') return Token(TokenType::OPERATOR, TokenAttribute::Gequal);	// >=
		retract();
		return Token(TokenType::OPERATOR, TokenAttribute::Greater);
	}
	else if (buffer[scanPoint] == '<') {
		step();
		if (buffer[scanPoint] == '=') return Token(TokenType::OPERATOR, TokenAttribute::Lequal);	// <=
		retract();
		return Token(TokenType::OPERATOR, TokenAttribute::Less);
	}
	else if (buffer[scanPoint] == '!' && buffer[scanPoint + 1] == '=') {
		step();
		return Token(TokenType::OPERATOR, TokenAttribute::Less);
	}
	else if (buffer[scanPoint] == ',') return Token(TokenType::BOUNDARY, TokenAttribute::Comma);
	else if (buffer[scanPoint] == ';') return Token(TokenType::BOUNDARY, TokenAttribute::Semicolon);
	else if (buffer[scanPoint] == '{') return Token(TokenType::BRACKET, TokenAttribute::LeftBrace);
	else if (buffer[scanPoint] == '}') return Token(TokenType::BRACKET, TokenAttribute::RightBrace);
	else if (buffer[scanPoint] == '(') return Token(TokenType::BRACKET, TokenAttribute::LeftBracket);
	else if (buffer[scanPoint] == ')') return Token(TokenType::BRACKET, TokenAttribute::RightBracket);
	else // 错误处理
		return Token(TokenType::FAIL, TokenAttribute::_fail);

}