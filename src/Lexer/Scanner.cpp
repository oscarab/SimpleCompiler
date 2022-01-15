#include "Lexer/Lexer.h"

#include <sstream>

// ָ��ǰ�Ʋ��ж��Ƿ���Ҫ����������
#define STEP_OR_REQUEST {if (step() == 0) { \
						rollback();\
						return Token(TokenType::INCOMPLETE, TokenAttribute::None);}}

using namespace Lexical;

extern std::vector<std::string> idTable;
extern std::vector<double> constantTable;
/* ɨ���� */

// ���캯��
Scanner::Scanner() {
	endPoint = 0;
	isComplete = 0;

	startPoint = -1;
	scanPoint = -1;

	buffer = NULL;
}

// ��ȡbuffer����
void Scanner::setBuffer(char* buffer) {
	this->buffer = buffer;
}

// ��ȡ����λ��
void Scanner::setEndPoint(int endPoint) {
	this->endPoint = endPoint;
}

// ��ȡ������
void Scanner::setIsComplete(int isComplete) {
	this->isComplete = isComplete;
}

// �ж�����
bool Scanner::isDigit() {
	return (buffer[scanPoint] >= '0' && buffer[scanPoint] <= '9');
}

// �ж���ĸ
bool Scanner::isLetter() {
	return ((buffer[scanPoint] >= 'a' && buffer[scanPoint] <= 'z') || (buffer[scanPoint] >= 'A' && buffer[scanPoint] <= 'Z'));
}

// �Ƿ��Ѿ�ɨ�����
int Scanner::isReachComplete() {
	if (scanPoint >= endPoint) {
		return isComplete;
	}
	return -1;
}

// ָ��ǰ��
int Scanner::step() {
	if (scanPoint + 1 >= endPoint) {
		scanPoint = (scanPoint + 1) % (2 * BLOCK_SIZE);
		return isComplete;
	}
	scanPoint = (scanPoint + 1) % (2 *BLOCK_SIZE);
	return isReachComplete();
}

// ָ�����
void Scanner::retract() {
	scanPoint--;
}

// ָ�����
void Scanner::align() {
	startPoint = ++scanPoint;
}

// �����Ѷ�����ַ�
void Scanner::rollback() {
	scanPoint = --startPoint;
}

// �����հ�
void Scanner::skipBlank() {
	while (buffer[scanPoint] == ' ') {
		startPoint++;
		scanPoint++;
	}
}

// ���ҹؼ��ֱ�
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

// �����±�ʶ��
int Scanner::insertID() {
	std::string str = std::string(buffer + startPoint, (scanPoint - startPoint) + 1);
	int index;

	if (tablePoint.find(str) == tablePoint.end()) {
		idTable.push_back(str);	// �����ʶ����
		index = idTable.size() - 1;
		tablePoint[str] = index;
	}
	else {
		index = tablePoint[str];
	}
	
	return index;
}

// �����³���
int Scanner::insertConstant() {
	std::string str = std::string(buffer + startPoint, (scanPoint - startPoint) + 1);
	int index;
	double constant;
	std::stringstream strstream;

	// �ַ���תʵ��
	strstream << str;
	strstream >> constant;

	if (tablePoint.find(str) == tablePoint.end()) {
		// ���볣����
		constantTable.push_back(constant);
		index = constantTable.size() - 1;
		tablePoint[str] = index;
	}
	else {
		index = tablePoint[str];
	}
	
	return index;
}

// ɨ��
Token Scanner::scan() {
	int value;					// ����ֵ
	TokenAttribute attr;		// �ؼ���
	align();					// ͳһָ��λ��
	skipBlank();				// �����հ�

	if (isReachComplete() == 1) {
		// �Ѿ����ɨ��
		return Token(TokenType::COMPLETE, TokenAttribute::None);
	}
	else if (isReachComplete() == 0) {
		// ���ܻ��к������ݣ���������
		startPoint = scanPoint = endPoint % (2 * BLOCK_SIZE) - 1;
		return Token(TokenType::INCOMPLETE, TokenAttribute::None);
	}

	// ������ĸ
	if (isLetter()) {
		while (isLetter() || isDigit()) {
			STEP_OR_REQUEST;
		}
		retract();
		attr = reserve();
		if (attr == TokenAttribute::RealID) {
			// �Զ����ʶ��
			value = insertID();
			return Token(TokenType::ID, TokenAttribute::RealID, value);
		}
		else {
			// �ؼ���
			return Token(TokenType::KEY_WORD, attr);
		}
	}
	else if (isDigit()) {	// ��������
		while (isDigit()) {
			// ��ȡ��������
			STEP_OR_REQUEST;
		}
		if (buffer[scanPoint] == '.') {
			STEP_OR_REQUEST;
			while (isDigit()) {
				// ��ȡС������
				STEP_OR_REQUEST;
			}
			if (buffer[scanPoint] == 'e' || buffer[scanPoint] == 'E') {
				STEP_OR_REQUEST;
				if (buffer[scanPoint] == '-') STEP_OR_REQUEST;
				if (!isDigit()) return Token(TokenType::FAIL, TokenAttribute::None);
				while (isDigit()) {
					// ��ȡָ������
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
		// ������
		return Token(TokenType::FAIL, TokenAttribute::None);
	}

}