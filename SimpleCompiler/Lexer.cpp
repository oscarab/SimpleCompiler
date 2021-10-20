#include "Lexer.h"

#include <iostream>

using namespace LexicalAnalysis;

/***********单词二元组***********/

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

/***********词法分析器***********/

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
			// 当前扫描未完成，可能尚有字符未读入
			if (!codeReader->read()) {
				// 告知扫描器已经完成读入
			}
			else {
				// 告知扫描器已读入新内容
				int readin = codeReader->getReadin();
			}
		}
		else if (token.getType() == TokenType::FAIL) {
			// 词法分析遭遇错误
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