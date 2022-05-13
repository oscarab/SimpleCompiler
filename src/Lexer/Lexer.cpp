#include "Lexer/Lexer.h"
#include "Output/Output.h"
#include "Output/Log.h"
#include "Output/Exception.h"
#include <iostream>

using namespace Lexical;

extern const char* TokenTypeStr[];
extern const char* TokenAttrStr[];
extern std::vector<std::string> idTable;		// ��ʶ����
extern std::vector<std::string> constantTable;	// ������
extern void tab(Log* log, int level);

/***********���ʶ�Ԫ��***********/

Token::Token(TokenType type, TokenAttribute attribute, int row, int col) : Token(type, attribute, -1, row, col){}

Token::Token(TokenType type, TokenAttribute attribute, int index, int row, int col)
			: type(type), attribute(attribute), index(index), row(row), col(col)
{ }

/**
 * @brief ��ȡ���ű��������±�
 * @return �±�
*/
int Token::getIndex() {
	return index;
}

/**
 * @brief ��ȡ����
 * @return ����
*/
TokenType Token::getType() {
	return type;
}

/**
 * @brief ��ȡ����
 * @return ����
*/
TokenAttribute Token::getAttribute() {
	return attribute;
}

/**
 * @brief �����Ƿ�Ϊ��
 * @return true ��Ϊ��
*/
bool Token::isEmpty() {
	return type == TokenType::EPSILON && attribute == TokenAttribute::None;
}

/**
 * @brief ���
 * @param level ����
 * @param package �����Ƿ��û����Ű�Χ
*/
void Token::write(Log* log, int level, bool package) {
	if (package) {
		tab(log, level);
		log->logln("{");
	}
	else {
		level--;
	}

	tab(log, level + 1);
	log->log("\"type\": \"")->log(TokenTypeStr[int(type)])->logln("\",");
	
	if (attribute == TokenAttribute::RealID) {
		tab(log, level + 1);
		log->log("\"attribute\": \"")->log(idTable[index])->logln("\",");
	}
	else if (attribute == TokenAttribute::RealConstant) {
		tab(log, level + 1);
		log->log("\"attribute\": \"")->log(constantTable[index])->logln("\",");
	}
	else {
		tab(log, level + 1);
		log->log("\"attribute\": \"")->log(TokenAttrStr[int(attribute)])->logln("\",");
	}

	if (package) {
		tab(log, level);
		log->logln("}");
	}
}

void Token::setIndex(int ind) {
	index = ind;
}

bool Token::operator==(const Token& token) const {
	return type == token.type && attribute == token.attribute;
}

/***********�ʷ�������***********/

Lexer::Lexer(const char* fileName) {
	codeReader = new Reader(fileName);
	scanner = new Scanner();
}

/**
 * @brief ���дʷ�����
 * @return �Ƿ�ɹ�
*/
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
			throw CompilerException("[ERROR] Lexical error!", token);
		}
		else if (token.getType() == TokenType::COMPLETE) {
			break;
		}
		else {
			tokens.push_back(token);
		}
	}

	// ��������ַ�
	tokens.push_back(Token(TokenType::END, TokenAttribute::None));
}

std::vector<Token>* Lexer::getTokens() {
	return &tokens;
}

void Lexer::writeTokens() {
	Log* log = FileLog::getInstance("token.txt");
	for (Token& token : tokens) {
		token.write(log, 0, true);
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
		std::cerr << "fail to open code file";
		exit(0);
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
		// ȥ��tab
		if (buffer[p] == '\t') {
			buffer[p] = ' ';
		}

		if (buffer[p++] == '/') {
			if (buffer[p] == '/') {
				// ��������ע�ͣ�ȫ����ȥ
				buffer[p] = buffer[p - 1] = ' ';
				while (buffer[++p] != '\n') {
					buffer[p] = ' ';
				}
			}
			else if (buffer[p] == '*') {
				// ��������ע��
				buffer[p] = buffer[p - 1] = ' ';
				p++;
				while (buffer[p] != '*' || buffer[p + 1] != '/') {
					if (buffer[p] == '\n') p++;
					else buffer[p++] = ' ';
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