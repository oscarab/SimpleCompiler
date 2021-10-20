#include "Lexer.h"

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