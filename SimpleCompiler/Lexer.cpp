#include "Lexer.h"

#include <iostream>

using namespace Lexical;

const char* TokenTypeStr[] = {"KEY_WORD", "ID", "OPERATOR", "CONSTANT", 
							"BOUNDARY", "BRACKET"};
const char* TokenAttrStr[] = {"$int", "$void", "$if", "$else", "$while", 
							"$return", "+", "-", "*", "/", "=", "==", 
							">", "<", ">=", "<=", "!=", ",", ";", 
							"{", "}", "(", ")"};

/***********���ʶ�Ԫ��***********/

Token::Token(TokenType type, TokenAttribute attribute) : Token(type, attribute, -1){}

Token::Token(TokenType _type, TokenAttribute _attribute, int _index) {
	type = _type;
	attribute = _attribute;
	index = _index;
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

bool Token::operator==(const Token& token) const {
	return type == token.type && attribute == token.attribute && index == token.index;
}

/***********�ʷ�������***********/

Lexer::Lexer(const char* fileName) {
	codeReader = new Reader(fileName);
	scanner = new Scanner(&idTable, &constantTable);
}


void Lexer::run() {
	scanner->setBuffer(codeReader->getBuffer());
	scanner->setEndPoint(codeReader->getReadin());
	scanner->setIsComplete(codeReader->getReadin() < BLOCK_SIZE);

	while (1) {
		Token token = scanner->scan();

		if (token.getType() == TokenType::INCOMPLETE) {
			// ��ǰɨ��δ��ɣ����������ַ�δ����
			if (!codeReader->read()) {
				// ��֪ɨ�����Ѿ���ɶ���
				scanner->setIsComplete(true);
			}
			else {
				// ��֪ɨ�����Ѷ���������
				int readin = codeReader->getReadin();
				scanner->setIsComplete(false);
				scanner->setEndPoint(codeReader->getScanPart() * BLOCK_SIZE + readin);
			}
		}
		else if (token.getType() == TokenType::FAIL) {
			// �ʷ�������������
			std::cout << "�ʷ�����" << std::endl;
			break;
		}
		else if (token.getType() == TokenType::COMPLETE) {
			break;
		}
		else {
			tokens.push_back(token);
		}
	}
}

std::vector<Token>* Lexer::getTokens() {
	return &tokens;
}

void Lexer::show() {
	for (Token& token : tokens) {
		std::cout << "<" << TokenTypeStr[int(token.getType())] << ", ";
		if (token.getAttribute() == TokenAttribute::RealID) {
			std::cout << idTable[token.getIndex()];
		}
		else if (token.getAttribute() == TokenAttribute::RealConstant) {
			std::cout << constantTable[token.getIndex()];
		}
		else {
			std::cout << TokenAttrStr[int(token.getAttribute())];
		}
		std::cout << ">" << std::endl;
	}
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
	pretreat();
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
		// ȥ��������tab
		if (buffer[p] == '\n' || buffer[p] == '\t') {
			buffer[p] = ' ';
		}

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
				buffer[p] = buffer[p + 1] = ' ';
				p += 2;
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

char* Reader::getBuffer() {
	return buffer;
}

Reader::~Reader() {
	delete[] buffer;
	fin.close();
}