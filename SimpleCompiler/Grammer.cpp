#define _CRT_SECURE_NO_WARNINGS
#include "Grammer.h"
#include <iostream>
#include <fstream>
#include <unordered_map>

Symbol::Symbol(Token _token) : token(_token){
	end = true;
}

Symbol::Symbol(String _str) : token(TokenType::END, TokenAttribute::None){
	name = _str;
	end = false;
}

bool Symbol::isEnd() const{
	return end;
}

String Symbol::getName() const {
	return name;
}

Token Symbol::getToken() const {
	return token;
}

std::vector<PSymbol>* Symbol::getProductions() {
	std::vector<PSymbol>* _p = &production;
	return _p;
}

void Symbol::insertProduction(PSymbol& psymbol) {
	production.push_back(psymbol);	// ���²���ʽ
	psymbol.clear();	//׼��������һ������ʽ
}

bool Symbol::operator==(const Symbol& symbol) const {
	return end == symbol.end && token == symbol.token && name == symbol.name;
}


// �Ƴ����ս���ļ�����
String Grammer::removeBrackets(String s) {
	return s.substr(1, s.size() - 2);
}

// �����ս���ַ���ΪToken��ʽ
Token Grammer::setToken(String s) {
	if (s == "int") return Token(TokenType::KEY_WORD, TokenAttribute::_int);
	else if (s == "void") return Token(TokenType::KEY_WORD, TokenAttribute::_void);
	else if (s == "if") return Token(TokenType::KEY_WORD, TokenAttribute::_if);
	else if (s == "else") return Token(TokenType::KEY_WORD, TokenAttribute::_else);
	else if (s == "while") return Token(TokenType::KEY_WORD, TokenAttribute::_while);
	else if (s == "return") return Token(TokenType::KEY_WORD, TokenAttribute::_return);
	else if (s == "id") return Token(TokenType::ID, TokenAttribute::RealID);
	else if (s == "num") return Token(TokenType::CONSTANT, TokenAttribute::RealConstant);
	else if (s == "+") return Token(TokenType::OPERATOR, TokenAttribute::Add);
	else if (s == "-") return Token(TokenType::OPERATOR, TokenAttribute::Minus);
	else if (s == "*") return Token(TokenType::OPERATOR, TokenAttribute::Multiply);
	else if (s == "/") return Token(TokenType::OPERATOR, TokenAttribute::Divide);
	else if (s == "<") return Token(TokenType::OPERATOR, TokenAttribute::Less);
	else if (s == ">") return Token(TokenType::OPERATOR, TokenAttribute::Greater);
	else if (s == "<=") return Token(TokenType::OPERATOR, TokenAttribute::Lequal);
	else if (s == ">=") return Token(TokenType::OPERATOR, TokenAttribute::Gequal);
	else if (s == "==") return Token(TokenType::OPERATOR, TokenAttribute::Equal);
	else if (s == "!=") return Token(TokenType::OPERATOR, TokenAttribute::Nequal);
	else if (s == ",") return Token(TokenType::BOUNDARY, TokenAttribute::Comma);
	else if (s == ";") return Token(TokenType::BOUNDARY, TokenAttribute::Semicolon);
	else if (s == "(") return Token(TokenType::BRACKET, TokenAttribute::LeftBrace);
	else if (s == ")") return Token(TokenType::BRACKET, TokenAttribute::RightBrace);
	else if (s == "{") return Token(TokenType::BRACKET, TokenAttribute::LeftBracket);
	else if (s == "}") return Token(TokenType::BRACKET, TokenAttribute::RightBracket);
	else if (s == "��") return Token(TokenType::EPSILON, TokenAttribute::None);
	else return Token(TokenType::END, TokenAttribute::None);
}

void Grammer::getProduction(int numSymbol, int numProduction, PSymbol& psymbol) {
	psymbol = (*(leftSymbols[numSymbol]->getProductions()))[numProduction];	// ��numSymbol�����ս������numProduction������ʽ
}

// ����ɿյķ��ս��
void Grammer::solveCanEmpty() {
	int temp = canEmpty.size();
	for (int i = 0; i < leftSymbols.size(); i++) {	// ÿ�������ս��
		//Symbol* tempSymbol = leftSymbols[i];
		for (int j = 0; j < (*leftSymbols[i]->getProductions()).size(); j++) {	// ��Ӧÿ������ʽ
			if (((*leftSymbols[i]->getProductions())[j])[0]->getToken() == Token(TokenType::END, TokenAttribute::None))	// ����ʽ���п�
				canEmpty.insert(std::pair<Symbol, bool>(*leftSymbols[i], true));	// ����ɿշ���
			else {	// ���������ԵĿ�
				for (int k = 0; k < (leftSymbols[i]->getProductions()[j]).size(); k++) {	// ����ʽ��ÿ������
					if (canEmpty.find(*((*leftSymbols[i]->getProductions())[j])[k]) == canEmpty.end())	// ����ʽ�о����ڿɿյķ��ս��
						break;
					canEmpty.insert(std::pair<Symbol, bool>(*leftSymbols[i], true));	// ����ɿշ���
				}
			}
		}
	}
	if (canEmpty.size() != temp)	// �ɿշ���δ�ȶ�����������
		solveCanEmpty();
}

// �������з��ս����First����
void Grammer::solveFirst() {
	bool flag = true;	// ��¼�Ƿ����ɹ�
	while (flag) {
		flag = false;
		for (int i = 0; i < leftSymbols.size(); i++) {	// ÿ�������ս��
			for (int j = 0; j < (*leftSymbols[i]->getProductions()).size(); j++) {	// ��Ӧÿ������ʽ
				Symbol* p = (*leftSymbols[i]->getProductions())[j][0];
				while (p) {
					if (p->isEnd()) {	// �����ս��
					// ֱ�Ӽӵ�first�����
						if (p->getToken().getType() != TokenType::EPSILON) {	// �����ǿ�
							if(firstSet[*leftSymbols[i]].insert(p).second == true) flag = true;	// �ɹ�����
						}
						break;
					}
					else {	// ���Ƿ��ս��
						if (firstSet.find(*p) == firstSet.end()) {	// ���ж�Ӧ��First��
							// ��First�������
							for (std::set<Symbol*>::iterator k = firstSet[*p].begin(); k != firstSet[*p].end(); ++k) {
								if(firstSet[*leftSymbols[i]].insert(*k).second == true) flag = true;
							}
						}
						if (canEmpty.find(*p) == canEmpty.end())	// �����ս���ɿ�
							p++;	// ����һ������
						else	// ���ɿ�
							break;
					}
				}
			}
		}
	}
}

void Grammer::solveFirst(PSymbol& psymbol) {
	if (psymbol[0]->isEnd() == true) {	// ��λΪ�ս����First��������
		Symbol* tempSymbol = psymbol[0];
		psymbol.clear();
		psymbol.push_back(tempSymbol);
	}
	else {	// ��λ���ս��
		psymbol.clear();
		for (std::set<Symbol*>::iterator i =
			(firstSet.find(*psymbol[0])->second).begin(); i != (firstSet.find(*psymbol[0])->second).end(); ++i)
			psymbol.push_back(*i);
	}
}

int Grammer::getSymbolIndex(Symbol* symbol) {
	return symMapTable.find(*symbol)->second;
}

PSymbol* Grammer::getSymbols() {
	return &symbols;
}

Grammer::Grammer(const char* filename) {
	// ��ȡ�������ķ��ļ� Grammer.txt
	std::ifstream grammerFile(filename);
	if (!grammerFile.is_open())
		std::cout << "���ķ��ļ�ʧ�ܣ�" << std::endl;

	String temp;
	while (getline(grammerFile, temp)) {	// ��ÿһ��
		// ��¼�����ս��
		int pos = temp.find("::=");
		String s = temp.substr(0, pos);
		Symbol* tempSymbol = new Symbol(removeBrackets(s));	// "::="�����ս�� Symbol(String)
		if (symMapTable.find(*tempSymbol) == symMapTable.end()) {	// û��������
			symbols.push_back(tempSymbol);	// ���з���
			strMapTable.insert(std::pair<String, int>(s, symbols.size() - 1));	// �ַ������±�
			symMapTable.insert(std::pair<Symbol, int>(*tempSymbol, symbols.size() - 1));	// �������±�
		}
		leftSymbols.push_back(tempSymbol);	// �����󲿷���

		// ��¼�Ҳ����
		int pos1 = pos+3, pos2 = pos1;
		PSymbol production;	// �洢����ʽ
		while (pos1 <= temp.size()) {
			if (temp[pos1] == '<') {	// �������ս��
				while (temp[pos2] != '>') pos2++;
				s = temp.substr(pos1 + 1, pos2 - pos1 - 1);
				Symbol* _tempSymbol = new Symbol(removeBrackets(s));
				production.push_back(_tempSymbol);	// ���µ�ǰ����ʽ
				// ���ս��ֻ����Ϊ�󲿷���ʱ�����ϣ��
				delete _tempSymbol;
				pos2++, pos1 = pos2;
			}
			else if(temp[pos1] == '"') {	// �����ս��
				while (temp[pos2] != '"') pos2++;
				s = temp.substr(pos1 + 1, pos2 - pos1 - 1);
				Symbol* _tempSymbol = new Symbol(setToken(s));
				production.push_back(_tempSymbol);	// ���µ�ǰ����ʽ
				if (symMapTable.find(*_tempSymbol) == symMapTable.end()) {	// û��������
					symbols.push_back(_tempSymbol);	// ���з���
					strMapTable.insert(std::pair<String, int>(s, symbols.size() - 1));	// �ַ������±�
					symMapTable.insert(std::pair<Symbol, int>(*_tempSymbol, symbols.size() - 1));	// �������±�
				}
				else
					delete _tempSymbol;
				pos2++, pos1 = pos2;
			}
			else if (temp[pos1] == '|') {
				pos2++, pos1 = pos2;
				tempSymbol->insertProduction(production);	// ���뵱ǰ����ʽ����գ�׼����ȡ��һ������ʽ
			}
		}
		tempSymbol->insertProduction(production);	// ĩβ���뵱ǰ����ʽ�����
	}
	grammerFile.close();
}

Grammer::~Grammer() {
	
}