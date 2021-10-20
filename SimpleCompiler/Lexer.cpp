#include "Lexer.h"
//#include <unordered_set>

#include <iostream>

using namespace LexicalAnalysis;

/***********���ʶ�Ԫ��***********/

Token::Token(TokenType type, TokenAttribute attribute) {
	Token(type, attribute, -1);
}

Token::Token(TokenType type, TokenAttribute attribute, int index) {
	this->type = type;
	this->attribute = attribute;
	this->index = index;
}

int Token::getIndex() {
	return index;
}

TokenType Token::getType() {
	return type;
}

TokenAttribute Token::getAttribute() {
	return attribute;
}

std::ostream& operator<<(std::ostream& cout, Token& token) {
	cout << "<" << int(token.getType()) << ", " << int(token.getAttribute()) << ">" << std::endl;
}

/***********�ʷ�������***********/

Lexer::Lexer(const char* fileName) {
	idTable = std::vector<std::string>(126);
	constantTable = std::vector<double>(126);

	codeReader = new Reader(fileName);
	scanner = new Scanner(&idTable, &constantTable);
}

void Lexer::run() {
	while (1) {
		Token token = scanner->scan();

		if (token.getType() == TokenType::INCOMPLETE) {
			// ��ǰɨ��δ��ɣ����������ַ�δ����
			if (!codeReader->read()) {
				// ��֪ɨ�����Ѿ���ɶ���
			}
			else {
				// ��֪ɨ�����Ѷ���������
				int readin = codeReader->getReadin();
			}
		}
		else if (token.getType() == TokenType::FAIL) {
			// �ʷ�������������
			break;
		}
	}
}

std::vector<Token> Lexer::getTokens() {
	return tokens;
}

Lexer::~Lexer() {
	delete codeReader;
	delete scanner;
}

/***********���ݶ�����***********/

Reader::Reader(const char* fileName) {
	// ��ǰɨ������Ϊǰ�벿��
	scanPart = FRONT;
	// ���뻺����
	buffer = new char[2 * BLOCK_SIZE];

	// ����1MB�Ĵ�������
	fin = std::ifstream(fileName, std::ios::in);
	if (!fin.is_open()) {
		std::cerr << "����Դ�����ļ���ʧ�ܣ�";
		exit(1);
	}
	fin.read(buffer, BLOCK_SIZE);

	// �����ֽ���
	readin = int(fin.gcount());
}

/*
* ����Դ�����
* ��������
* ���أ�true ��ȡ�ɹ�
*/
bool Reader::read() {
	scanPart = !scanPart;

	fin.read(buffer + scanPart * BLOCK_SIZE, BLOCK_SIZE);
	if ((readin = int(fin.gcount())) == 0) {
		return false;
	}

	pretreat();
	return true;
}

/*
* Ԥ����ģ��
* ȥ��Դ�����е�ע��
*/
void Reader::pretreat() {
	int start = scanPart * BLOCK_SIZE;
	int p = start;

	while (p < start + readin) {
		if (buffer[p++] == '/') {
			if (buffer[p] == '/') {
				// ��������ע�ͣ�ȫ����ȥ
				buffer[p] = buffer[p - 1] = ' ';
				while (buffer[++p] != '\n') {
					buffer[p] = ' ';
				}
				buffer[p++] = ' ';
			}
			else if (buffer[p] == '*') {
				// ��������ע��
				buffer[p] = buffer[p - 1] = ' ';
				p++;
				while (buffer[p] != '*' || buffer[p + 1] != '/') {
					buffer[p++] = ' ';
				}
				buffer[p] = buffer[p - 1] = ' ';
			}
		}
	}
}

int Reader::getReadin() {
	return readin;
}

bool Reader::getScanPart() {
	return scanPart;
}

Reader::~Reader() {
	delete[] buffer;
}

/* ɨ���� */

// ���캯��
Scanner::Scanner(std::vector<std::string>* id_table, std::vector<double>* constant_table) {
	idTable = id_table;
	constantTable = constant_table;

	endPoint = 0;
	isComplete = 0;

	startPoint = -1;
	scanPoint = -1;
	// ��ϣ��
	//std::unordered_set<std::string> hashsetTokenType;
	//std::unordered_set<std::string> hashsetTokenAttribute;

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

// ָ��ǰ��
void Scanner::step() {
	scanPoint++;
}

// ָ�����
void Scanner::retract() {
	//if(!startPoint)
	scanPoint--;
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

// �����±�ʶ��
int Scanner::insertID() {
	std::string str = buffer;
	str = str.substr(startPoint + 1, (scanPoint - startPoint));
	idTable->push_back(str);	// �����ʶ����
	return idTable->size() - 1;
}

// �����³���
int Scanner::insertConstant() {
	std::string str = buffer;
	str = str.substr(startPoint + 1, (scanPoint - startPoint));
	double constant = atof(str.c_str());
	constantTable->push_back(constant);	// ���볣����
	return constantTable->size() - 1;
}

// ��װToken
//Token Scanner::createToken() {
//}

// ɨ��
Token Scanner::scan() {
	int value;	// ����ֵ
	TokenAttribute attr;	// �ؼ���
	startPoint = scanPoint;	// ͳһָ��λ��

	step();	// ǰ����ɨ��һ���ַ�
	if (scanPoint == endPoint) {
		if(isComplete)
			return Token(TokenType::COMPLETE, TokenAttribute::_complete);
		else
			return Token(TokenType::INCOMPLETE, TokenAttribute::_incomplete);
	}
	skipBlank();	// �����հ�

	// ������ĸ
	if (isLetter()) {
		while (isLetter() || isDigit())
			step();
		retract();
		attr = reserve();
		if (attr == TokenAttribute::_others) {	// �Զ����ʶ��
			value = insertID();
			return Token(TokenType::ID, TokenAttribute::RealString, value);
		}
		else // �ؼ���
			return Token(TokenType::KEY_WORD, attr);
	}
	else if (isDigit()) {	// ��������
		while (isDigit())
			step(); // С��������
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
	else // ������
		return Token(TokenType::FAIL, TokenAttribute::_fail);

}