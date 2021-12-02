#include "Lexer.h"
#include <iostream>

using namespace Lexical;

extern const char* TokenTypeStr[];
extern const char* TokenAttrStr[];
extern std::vector<std::string> idTable;	// ��ʶ����
extern std::vector<double> constantTable;	// ������
extern void tab(std::ostream& out, int level);

/***********���ʶ�Ԫ��***********/

Token::Token(TokenType type, TokenAttribute attribute) : Token(type, attribute, -1){}

Token::Token(TokenType _type, TokenAttribute _attribute, int _index) {
	type = _type;
	attribute = _attribute;
	index = _index;
}

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
 * @param out ���Դ
 * @param level ����
 * @param package �����Ƿ��û����Ű�Χ
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

/***********�ʷ�������***********/

Lexer::Lexer(const char* fileName) {
	codeReader = new Reader(fileName);
	scanner = new Scanner();
}

/**
 * @brief ���дʷ�����
*/
bool Lexer::run() {
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

	// ��������ַ�
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