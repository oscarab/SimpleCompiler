#include "Lexer.h"
#include <iostream>

using namespace Lexical;

extern const char* TokenTypeStr[];
extern const char* TokenAttrStr[];
extern std::vector<std::string> idTable;	// 标识符表
extern std::vector<double> constantTable;	// 常数表
extern void tab(std::ostream& out, int level);

/***********单词二元组***********/

Token::Token(TokenType type, TokenAttribute attribute) : Token(type, attribute, -1){}

Token::Token(TokenType _type, TokenAttribute _attribute, int _index) {
	type = _type;
	attribute = _attribute;
	index = _index;
}

/**
 * @brief 获取符号表或常数表的下标
 * @return 下标
*/
int Token::getIndex() {
	return index;
}

/**
 * @brief 获取种类
 * @return 种类
*/
TokenType Token::getType() {
	return type;
}

/**
 * @brief 获取属性
 * @return 属性
*/
TokenAttribute Token::getAttribute() {
	return attribute;
}

/**
 * @brief 单词是否为空
 * @return true 若为空
*/
bool Token::isEmpty() {
	return type == TokenType::EPSILON && attribute == TokenAttribute::None;
}

/**
 * @brief 输出
 * @param out 输出源
 * @param level 缩进
 * @param package 最外是否用花括号包围
*/
void Token::write(std::ostream& out, int level, bool package) {
	if (package) {
		tab(out, level);
		out << "{" << std::endl;
	}
	else {
		level--;
	}

	tab(out, level + 1);
	out << "\"type\": \"" << TokenTypeStr[int(type)] << "\"," << std::endl;
	
	if (attribute == TokenAttribute::RealID) {
		tab(out, level + 1);
		out << "\"attribute\": \"" << idTable[index] << "\"," << std::endl;
	}
	else if (attribute == TokenAttribute::RealConstant) {
		tab(out, level + 1);
		out << "\"attribute\": \"" << constantTable[index] << "\"," << std::endl;
	}
	else {
		tab(out, level + 1);
		out << "\"attribute\": \"" << TokenAttrStr[int(attribute)] << "\"" << "," << std::endl;
	}

	if (package) {
		tab(out, level);
		out << "}" << std::endl;
	}
}

void Token::setIndex(int ind) {
	index = ind;
}

bool Token::operator==(const Token& token) const {
	return type == token.type && attribute == token.attribute;
}

/***********词法分析器***********/

Lexer::Lexer(const char* fileName) {
	codeReader = new Reader(fileName);
	scanner = new Scanner();
}

/**
 * @brief 运行词法分析
*/
bool Lexer::run() {
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
			std::cout << "lexical error!" << std::endl;
			return false;
		}
		else if (token.getType() == TokenType::COMPLETE) {
			break;
		}
		else {
			tokens.push_back(token);
		}
	}

	// 加入结束字符
	tokens.push_back(Token(TokenType::END, TokenAttribute::None));
	return true;
}

std::vector<Token>* Lexer::getTokens() {
	return &tokens;
}

void Lexer::writeTokens(std::ostream& out) {
	for (Token& token : tokens) {
		token.write(out, 0, true);
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
		std::cerr << "fail to open code file";
		exit(0);
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