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
	production.push_back(psymbol);	// 更新产生式
	psymbol.clear();	//准备读入下一条产生式
}

bool Symbol::operator==(const Symbol& symbol) const {
	return end == symbol.end && token == symbol.token && name == symbol.name;
}


// 移除非终结符的尖括号
String Grammer::removeBrackets(String s) {
	return s.substr(1, s.size() - 2);
}

// 设置终结符字符串为Token格式
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
	else if (s == "空") return Token(TokenType::EPSILON, TokenAttribute::None);
	else return Token(TokenType::END, TokenAttribute::None);
}

void Grammer::getProduction(int numSymbol, int numProduction, PSymbol& psymbol) {
	psymbol = (*(leftSymbols[numSymbol]->getProductions()))[numProduction];	// 第numSymbol个非终结符，第numProduction个产生式
}

// 计算可空的非终结符
void Grammer::solveCanEmpty() {
	int temp = canEmpty.size();
	for (int i = 0; i < leftSymbols.size(); i++) {	// 每个左侧非终结符
		//Symbol* tempSymbol = leftSymbols[i];
		for (int j = 0; j < (*leftSymbols[i]->getProductions()).size(); j++) {	// 对应每个产生式
			if (((*leftSymbols[i]->getProductions())[j])[0]->getToken() == Token(TokenType::END, TokenAttribute::None))	// 产生式中有空
				canEmpty.insert(std::pair<Symbol, bool>(*leftSymbols[i], true));	// 插入可空符表
			else {	// 不存在明显的空
				for (int k = 0; k < (leftSymbols[i]->getProductions()[j]).size(); k++) {	// 产生式中每个符号
					if (canEmpty.find(*((*leftSymbols[i]->getProductions())[j])[k]) == canEmpty.end())	// 产生式中均存在可空的非终结符
						break;
					canEmpty.insert(std::pair<Symbol, bool>(*leftSymbols[i], true));	// 插入可空符表
				}
			}
		}
	}
	if (canEmpty.size() != temp)	// 可空符表未稳定，继续计算
		solveCanEmpty();
}

// 计算所有非终结符的First集合
void Grammer::solveFirst() {
	bool flag = true;	// 记录是否插入成功
	while (flag) {
		flag = false;
		for (int i = 0; i < leftSymbols.size(); i++) {	// 每个左侧非终结符
			for (int j = 0; j < (*leftSymbols[i]->getProductions()).size(); j++) {	// 对应每个产生式
				Symbol* p = (*leftSymbols[i]->getProductions())[j][0];
				while (p) {
					if (p->isEnd()) {	// 若是终结符
					// 直接加到first集最后
						if (p->getToken().getType() != TokenType::EPSILON) {	// 不考虑空
							if(firstSet[*leftSymbols[i]].insert(p).second == true) flag = true;	// 成功插入
						}
						break;
					}
					else {	// 若是非终结符
						if (firstSet.find(*p) == firstSet.end()) {	// 若有对应的First集
							// 将First逐个加入
							for (std::set<Symbol*>::iterator k = firstSet[*p].begin(); k != firstSet[*p].end(); ++k) {
								if(firstSet[*leftSymbols[i]].insert(*k).second == true) flag = true;
							}
						}
						if (canEmpty.find(*p) == canEmpty.end())	// 若非终结符可空
							p++;	// 看下一个符号
						else	// 不可空
							break;
					}
				}
			}
		}
	}
}

void Grammer::solveFirst(PSymbol& psymbol) {
	if (psymbol[0]->isEnd() == true) {	// 首位为终结符，First就是自身
		Symbol* tempSymbol = psymbol[0];
		psymbol.clear();
		psymbol.push_back(tempSymbol);
	}
	else {	// 首位非终结符
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
	// 读取并分析文法文件 Grammer.txt
	std::ifstream grammerFile(filename);
	if (!grammerFile.is_open())
		std::cout << "打开文法文件失败！" << std::endl;

	String temp;
	while (getline(grammerFile, temp)) {	// 读每一行
		// 记录左侧非终结符
		int pos = temp.find("::=");
		String s = temp.substr(0, pos);
		Symbol* tempSymbol = new Symbol(removeBrackets(s));	// "::="左侧非终结符 Symbol(String)
		if (symMapTable.find(*tempSymbol) == symMapTable.end()) {	// 没有遇到过
			symbols.push_back(tempSymbol);	// 所有符号
			strMapTable.insert(std::pair<String, int>(s, symbols.size() - 1));	// 字符串与下标
			symMapTable.insert(std::pair<Symbol, int>(*tempSymbol, symbols.size() - 1));	// 符号与下标
		}
		leftSymbols.push_back(tempSymbol);	// 所有左部符号

		// 记录右侧符号
		int pos1 = pos+3, pos2 = pos1;
		PSymbol production;	// 存储产生式
		while (pos1 <= temp.size()) {
			if (temp[pos1] == '<') {	// 读到非终结符
				while (temp[pos2] != '>') pos2++;
				s = temp.substr(pos1 + 1, pos2 - pos1 - 1);
				Symbol* _tempSymbol = new Symbol(removeBrackets(s));
				production.push_back(_tempSymbol);	// 更新当前产生式
				// 非终结符只在作为左部符号时加入哈希表
				delete _tempSymbol;
				pos2++, pos1 = pos2;
			}
			else if(temp[pos1] == '"') {	// 读到终结符
				while (temp[pos2] != '"') pos2++;
				s = temp.substr(pos1 + 1, pos2 - pos1 - 1);
				Symbol* _tempSymbol = new Symbol(setToken(s));
				production.push_back(_tempSymbol);	// 更新当前产生式
				if (symMapTable.find(*_tempSymbol) == symMapTable.end()) {	// 没有遇到过
					symbols.push_back(_tempSymbol);	// 所有符号
					strMapTable.insert(std::pair<String, int>(s, symbols.size() - 1));	// 字符串与下标
					symMapTable.insert(std::pair<Symbol, int>(*_tempSymbol, symbols.size() - 1));	// 符号与下标
				}
				else
					delete _tempSymbol;
				pos2++, pos1 = pos2;
			}
			else if (temp[pos1] == '|') {
				pos2++, pos1 = pos2;
				tempSymbol->insertProduction(production);	// 插入当前产生式并清空，准备读取下一条产生式
			}
		}
		tempSymbol->insertProduction(production);	// 末尾插入当前产生式并清空
	}
	grammerFile.close();
}

Grammer::~Grammer() {
	
}