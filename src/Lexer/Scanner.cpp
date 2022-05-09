#include "Lexer/Lexer.h"

// ָ��ǰ�Ʋ��ж��Ƿ���Ҫ����������
#define STEP_OR_REQUEST {if (step() == 0) { \
						rollback();\
						return Token(TokenType::INCOMPLETE, TokenAttribute::None, row, column);}}
#define NEW_LINE row++, column = 0;
#define MOVE_COL(a) column += a;
#define BACK_COL(a) column -= a;

using namespace Lexical;

extern std::vector<std::string> idTable;
extern std::vector<std::string> constantTable;
/* ɨ���� */

// ���캯��
Scanner::Scanner() {
	endPoint = 0;
	isComplete = 0;

	startPoint = -1;
	scanPoint = -1;

	buffer = NULL;

	row = column = 0;
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
	MOVE_COL(1)
	if (scanPoint + 1 >= endPoint) {
		scanPoint = (scanPoint + 1) % (2 * BLOCK_SIZE);
		return isComplete;
	}
	scanPoint = (scanPoint + 1) % (2 * BLOCK_SIZE);
	return isReachComplete();
}

// ָ�����
void Scanner::retract() {
	BACK_COL(1)
	scanPoint--;
}

// ָ�����
void Scanner::align() {
	MOVE_COL(1)
	startPoint = ++scanPoint;
}

// �����Ѷ�����ַ�
void Scanner::rollback() {
	BACK_COL(scanPoint - startPoint - 1)
	scanPoint = --startPoint;
}

// �����հ�
void Scanner::skipBlank() {
	while (buffer[scanPoint] == ' ' || buffer[scanPoint] == '\n') {
		MOVE_COL(1)
		if (buffer[scanPoint] == '\n') {
			NEW_LINE
		}

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
	
	if (tablePoint.find(str) == tablePoint.end()) {
		// ���볣����
		constantTable.push_back(str);
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
		return Token(TokenType::COMPLETE, TokenAttribute::None, row, column);
	}
	else if (isReachComplete() == 0) {
		// ���ܻ��к������ݣ���������
		startPoint = scanPoint = endPoint % (2 * BLOCK_SIZE) - 1;
		return Token(TokenType::INCOMPLETE, TokenAttribute::None, row, column);
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
			return Token(TokenType::ID, TokenAttribute::RealID, value, row, column);
		}
		else {
			// �ؼ���
			return Token(TokenType::KEY_WORD, attr, row, column);
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
				if (!isDigit()) return Token(TokenType::FAIL, TokenAttribute::None, row, column);
				while (isDigit()) {
					// ��ȡָ������
					STEP_OR_REQUEST;
				}
			}
		}
		retract();
		value = insertConstant();
		return Token(TokenType::CONSTANT, TokenAttribute::RealConstant, value, row, column);
	}
	else if (buffer[scanPoint] == '+') return Token(TokenType::OPERATOR, TokenAttribute::Add, row, column);
	else if (buffer[scanPoint] == '-') return Token(TokenType::OPERATOR, TokenAttribute::Minus, row, column);
	else if (buffer[scanPoint] == '*') return Token(TokenType::OPERATOR, TokenAttribute::Multiply, row, column);
	else if (buffer[scanPoint] == '/') return Token(TokenType::OPERATOR, TokenAttribute::Divide, row, column);
	else if (buffer[scanPoint] == '=') {
		STEP_OR_REQUEST;
		if (buffer[scanPoint] == '=') {
			return Token(TokenType::OPERATOR, TokenAttribute::Equal, row, column);
		}
		retract();
		return Token(TokenType::OPERATOR, TokenAttribute::Assign, row, column);
	}
	else if (buffer[scanPoint] == '>') {
		STEP_OR_REQUEST;
		if (buffer[scanPoint] == '=') return Token(TokenType::OPERATOR, TokenAttribute::Gequal, row, column);	// >=
		retract();
		return Token(TokenType::OPERATOR, TokenAttribute::Greater, row, column);
	}
	else if (buffer[scanPoint] == '<') {
		STEP_OR_REQUEST;
		if (buffer[scanPoint] == '=') return Token(TokenType::OPERATOR, TokenAttribute::Lequal, row, column);	// <=
		retract();
		return Token(TokenType::OPERATOR, TokenAttribute::Less, row, column);
	}
	else if (buffer[scanPoint] == '!') {
		STEP_OR_REQUEST;
		if (buffer[scanPoint] == '=') {
			return Token(TokenType::OPERATOR, TokenAttribute::Nequal, row, column);
		}
		else {
			return Token(TokenType::FAIL, TokenAttribute::None, row, column);
		}
	}
	else if (buffer[scanPoint] == '&') {
		STEP_OR_REQUEST;
		if (buffer[scanPoint] == '&') {
			return Token(TokenType::OPERATOR, TokenAttribute::AND, row, column);
		}
		else {
			return Token(TokenType::FAIL, TokenAttribute::None, row, column);
		}
	}
	else if (buffer[scanPoint] == '|') {
		STEP_OR_REQUEST;
		if (buffer[scanPoint] == '|') {
			return Token(TokenType::OPERATOR, TokenAttribute::OR, row, column);
		}
		else {
			return Token(TokenType::FAIL, TokenAttribute::OR, row, column);
		}
	}
	else if (buffer[scanPoint] == ',') return Token(TokenType::BOUNDARY, TokenAttribute::Comma, row, column);
	else if (buffer[scanPoint] == ';') return Token(TokenType::BOUNDARY, TokenAttribute::Semicolon, row, column);
	else if (buffer[scanPoint] == '{') return Token(TokenType::BRACKET, TokenAttribute::LeftBrace, row, column);
	else if (buffer[scanPoint] == '}') return Token(TokenType::BRACKET, TokenAttribute::RightBrace, row, column);
	else if (buffer[scanPoint] == '(') return Token(TokenType::BRACKET, TokenAttribute::LeftBracket, row, column);
	else if (buffer[scanPoint] == ')') return Token(TokenType::BRACKET, TokenAttribute::RightBracket, row, column);
	else if (buffer[scanPoint] == '[') return Token(TokenType::BRACKET, TokenAttribute::LeftSquare, row, column);
	else if (buffer[scanPoint] == ']') return Token(TokenType::BRACKET, TokenAttribute::RightSquare, row, column);
	else {
		// ������
		return Token(TokenType::FAIL, TokenAttribute::None, row, column);
	}

}