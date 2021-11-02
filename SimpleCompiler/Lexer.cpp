#include "Lexer.h"

#include <iostream>

using namespace Lexical;

const char* TokenTypeStr[] = {"KEY_WORD", "ID", "OPERATOR", "CONSTANT", 
							"BOUNDARY", "BRACKET"};
const char* TokenAttrStr[] = {"$int", "$void", "$if", "$else", "$while", 
							"$return", "+", "-", "*", "/", "=", "==", 
							">", "<", ">=", "<=", "!=", ",", ";", 
							"{", "}", "(", ")"};

/***********单词二元组***********/

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

/***********词法分析器***********/

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
			// 当前扫描未完成，可能尚有字符未读入
			if (!codeReader->read()) {
				// 告知扫描器已经完成读入
				scanner->setIsComplete(true);
			}
			else {
				// 告知扫描器已读入新内容
				int readin = codeReader->getReadin();
				scanner->setIsComplete(false);
				scanner->setEndPoint(codeReader->getScanPart() * BLOCK_SIZE + readin);
			}
		}
		else if (token.getType() == TokenType::FAIL) {
			// 词法分析遭遇错误
			std::cout << "词法错误" << std::endl;
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

/***********数据读入器***********/

Reader::Reader(const char* fileName) {
	// 当前扫描区域为前半部分
	scanPart = FRONT;
	// 申请缓冲区
	buffer = new char[2 * BLOCK_SIZE];

	// 读入1MB的代码数据
	fin = std::ifstream(fileName, std::ios::in);
	if (!fin.is_open()) {
		std::cerr << "输入源代码文件打开失败！";
		exit(1);
	}
	fin.read(buffer, BLOCK_SIZE);

	// 读入字节数
	readin = int(fin.gcount());
	pretreat();
}

/*
* 读入源代码块
* 参数：无
* 返回：true 读取成功
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
* 预处理模块
* 去除源代码中的注释
*/
void Reader::pretreat() {
	int start = scanPart * BLOCK_SIZE;
	int p = start;

	while (p < start + readin) {
		// 去除换行与tab
		if (buffer[p] == '\n' || buffer[p] == '\t') {
			buffer[p] = ' ';
		}

		if (buffer[p++] == '/') {
			if (buffer[p] == '/') {
				// 遇到单行注释，全行消去
				buffer[p] = buffer[p - 1] = ' ';
				while (buffer[++p] != '\n') {
					buffer[p] = ' ';
				}
				buffer[p++] = ' ';
			}
			else if (buffer[p] == '*') {
				// 遇到多行注释
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