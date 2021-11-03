#include "Grammer.h"
#include <iostream>
#include <fstream>
#include <unordered_map>

const std::unordered_map<String, Token> tokenConvert{ 
	{"int", Token(TokenType::KEY_WORD, TokenAttribute::_int)},
	{"void", Token(TokenType::KEY_WORD, TokenAttribute::_void)},
	{"if", Token(TokenType::KEY_WORD, TokenAttribute::_if)},
	{"else", Token(TokenType::KEY_WORD, TokenAttribute::_else)},
	{"while", Token(TokenType::KEY_WORD, TokenAttribute::_while)},
	{"return", Token(TokenType::KEY_WORD, TokenAttribute::_return)},
	{"id", Token(TokenType::ID, TokenAttribute::RealID)},
	{"num", Token(TokenType::CONSTANT, TokenAttribute::RealConstant)},
	{"=", Token(TokenType::OPERATOR, TokenAttribute::Assign)},
	{"+", Token(TokenType::OPERATOR, TokenAttribute::Add)},
	{"-", Token(TokenType::OPERATOR, TokenAttribute::Minus)},
	{"*", Token(TokenType::OPERATOR, TokenAttribute::Multiply)},
	{"/", Token(TokenType::OPERATOR, TokenAttribute::Divide)},
	{"<", Token(TokenType::OPERATOR, TokenAttribute::Less)},
	{">", Token(TokenType::OPERATOR, TokenAttribute::Greater)},
	{"<=", Token(TokenType::OPERATOR, TokenAttribute::Lequal)},
	{">=", Token(TokenType::OPERATOR, TokenAttribute::Gequal)},
	{"==", Token(TokenType::OPERATOR, TokenAttribute::Equal)},
	{"!=", Token(TokenType::OPERATOR, TokenAttribute::Nequal)},
	{",", Token(TokenType::BOUNDARY, TokenAttribute::Comma)},
	{";", Token(TokenType::BOUNDARY, TokenAttribute::Semicolon)},
	{"{", Token(TokenType::BRACKET, TokenAttribute::LeftBrace)},
	{"}", Token(TokenType::BRACKET, TokenAttribute::RightBrace)},
	{"(", Token(TokenType::BRACKET, TokenAttribute::LeftBracket)},
	{")", Token(TokenType::BRACKET, TokenAttribute::RightBracket)},
	{"#", Token(TokenType::END, TokenAttribute::None)}
};

Symbol::Symbol(Token _token) : token(_token){
	end = true;
}

Symbol::Symbol(String str) : token(TokenType::END, TokenAttribute::None){
	name = str;
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
	return &production;
}

void Symbol::insertProduction(PSymbol& psymbol) {
	production.push_back(psymbol);	// 更新产生式
	psymbol.clear();				//准备读入下一条产生式
}

bool Symbol::operator==(const Symbol& symbol) const {
	return end == symbol.end && token == symbol.token && name == symbol.name;
}

bool Symbol::operator<(const Symbol& symbol) const {
	if (end != symbol.end) {
		return end < symbol.end;
	}

	if (token == symbol.token) {
		return name < symbol.name;
	}
	else {
		return token < symbol.token;
	}
}


// 移除非终结符的尖括号
String removeBrackets(String& str) {
	return str.substr(1, str.size() - 2);
}

// 设置终结符字符串为Token格式
Token Grammer::setToken(String& str) {
	auto index = tokenConvert.find(str);
	if (index == tokenConvert.end()) {
		return Token(TokenType::END, TokenAttribute::None);
	}
	else {
		return (*index).second;
	}
}

void Grammer::getProduction(int numSymbol, int numProduction, PSymbol& psymbol) {
	// 获取第numSymbol个非终结符，第numProduction个产生式
	PSymbol& product = (*symbols[numSymbol]->getProductions())[numProduction];
	psymbol.insert(psymbol.end(), product.begin(), product.end());
}

// 计算可空的非终结符
void Grammer::solveCanEmpty() {
	int sym_size;

	do {
		sym_size = canEmpty.size();
		// 每个左侧非终结符
		for (int i = 0; i < leftSymbols.size(); i++) {
			std::vector<PSymbol>* prod = leftSymbols[i]->getProductions();
			int prod_size = prod->size();

			// 对应每个产生式
			for (int j = 0; j < prod_size; j++) {
				if ((*prod)[j][0]->getToken() == Token(TokenType::EPSILON, TokenAttribute::None)) {	// 产生式中有空
					// 插入可空符表
					canEmpty[*leftSymbols[i]] = true;
				}
				else {
					// 不存在明显的空
					int len = (*prod)[j].size();

					// 遍历产生式中每个符号
					for (int k = 0; k < len; k++) {
						if (!canEmpty.count(*(*prod)[j][k])) {
							break;
						}
						if (k == len - 1) {
							canEmpty[*leftSymbols[i]] = true;
						}
					}
				}
			}
		}
	} while (sym_size != canEmpty.size());
}

// 计算所有非终结符的First集合
void Grammer::solveFirst() {
	bool flag = true;	// 记录是否插入成功
	while (flag) {
		flag = false;
		for (int i = 0; i < leftSymbols.size(); i++) {	// 每个左侧非终结符
			for (int j = 0; j < (*leftSymbols[i]->getProductions()).size(); j++) {	// 对应每个产生式
				PSymbol& prod = (*leftSymbols[i]->getProductions())[j];
				int len = prod.size();
				for (int k = 0; k < len; k++) {
					Symbol* p = prod[k];

					if (p->isEnd()) {	// 若是终结符
					// 直接加到first集最后
						if (p->getToken().getType() != TokenType::EPSILON) {	// 不考虑空
							if (firstSet[*leftSymbols[i]].insert(p).second == true) flag = true;	// 成功插入
						}
						break;
					}
					else {	// 若是非终结符
						if (firstSet.find(*p) != firstSet.end()) {	// 若有对应的First集
							// 将First逐个加入
							for (auto iter = firstSet[*p].begin(); iter != firstSet[*p].end(); ++iter) {
								if (firstSet[*leftSymbols[i]].insert(*iter).second == true) flag = true;
							}
						}
						if (canEmpty.count(*p))	// 若非终结符可空
							p++;	// 看下一个符号
						else	// 不可空
							break;
					}
				}
			}
		}
	}
}

void Grammer::solveFirst(PSymbol& symbolStr) {
	PSymbol temp(symbolStr);
	symbolStr.clear();

	// 遍历整个串
	int len = temp.size();
	for (int i = 0; i < len; i++) {
		// 中途遇到终结符，计算结束
		if (temp[i]->isEnd()) {
			symbolStr.push_back(temp[i]);
			break;
		}

		std::set<Symbol*>::iterator begin = (firstSet[*temp[i]]).begin();
		std::set<Symbol*>::iterator end = (firstSet[*temp[i]]).end();
		for (auto i = begin; i != end; ++i) {
			symbolStr.push_back(*i);
		}

		// 若该非终结符不可空则计算结束
		if (canEmpty.find(*temp[i]) == canEmpty.end()) {
			break;
		}
	}
}

int Grammer::getSymbolIndex(Symbol* symbol) {
	return symMapTable.find(*symbol)->second;
}

PSymbol* Grammer::getSymbols() {
	return &symbols;
}

int Grammer::getProductionCount(int index) {
	return prodCounter[index];
}

Grammer::Grammer(const char* filename) {
	// 读取并分析文法文件 Grammer.txt
	std::ifstream grammerFile(filename);
	if (!grammerFile.is_open())
		std::cout << "打开文法文件失败！" << std::endl;

	String line;
	while (getline(grammerFile, line)) {	// 读每一行
		// 记录左侧非终结符
		int pos = line.find("::=");
		String s = line.substr(0, pos);
		Symbol* symbol;

		if (strMapTable.find(removeBrackets(s)) == strMapTable.end()) {
			symbol = new Symbol(removeBrackets(s));				// "::="左侧非终结符 Symbol(String)
			symbols.push_back(symbol);							// 所有符号
			strMapTable[removeBrackets(s)] = symbols.size() - 1;	// 字符串与下标
			symMapTable[*symbol] = symbols.size() - 1;			// 符号与下标
			leftSymbols.push_back(symbol);						// 所有左部符号
		}
		else {
			symbol = symbols[strMapTable[removeBrackets(s)]];
			leftSymbols.push_back(symbol);
		}

		// 记录右侧符号
		int pos1 = pos + 3, pos2 = pos1 + 1;
		PSymbol production;	// 存储产生式
		while (pos1 < line.size()) {
			if (line[pos1] == '<') {	// 读到非终结符
				while (line[pos2] != '>') pos2++;
				s = line.substr(pos1 + 1, pos2 - pos1 - 1);

				if (strMapTable.find(s) == strMapTable.end()) {	// 没有遇到过
					Symbol* _tempSymbol = new Symbol(s);
					production.push_back(_tempSymbol);	// 更新当前产生式
					symbols.push_back(_tempSymbol);	// 所有符号
					strMapTable.insert(std::pair<String, int>(s, symbols.size() - 1));	// 字符串与下标
					symMapTable.insert(std::pair<Symbol, int>(*_tempSymbol, symbols.size() - 1));	// 符号与下标
				}
				else {	// 遇到过
					production.push_back(symbols[strMapTable[s]]);
				}
				// 非终结符只在作为左部符号时加入哈希表
				pos1 = pos2 + 1;
				pos2 = pos1 + 1;
			}
			else if(line[pos1] == '"') {	// 读到终结符
				while (line[pos2] != '"') pos2++;
				s = line.substr(pos1 + 1, pos2 - pos1 - 1);

				if (strMapTable.find(s) == strMapTable.end()) {	// 没有遇到过
					Symbol* _tempSymbol = new Symbol(setToken(s));
					production.push_back(_tempSymbol);	// 更新当前产生式
					symbols.push_back(_tempSymbol);	// 所有符号
					strMapTable.insert(std::pair<String, int>(s, symbols.size() - 1));	// 字符串与下标
					symMapTable.insert(std::pair<Symbol, int>(*_tempSymbol, symbols.size() - 1));	// 符号与下标
				}
				else {	// 遇到过
					production.push_back(symbols[strMapTable[s]]);
				}
				pos1 = pos2 + 1;
				pos2 = pos1 + 1;
			}
			else if (line[pos1] == '|') {
				pos1++;
				pos2++;
				symbol->insertProduction(production);	// 插入当前产生式，准备读取下一条产生式
			}
		}
		symbol->insertProduction(production);	// 末尾插入当前产生式
	}
	grammerFile.close();

	prodCounter.push_back(0);
	for (Symbol* sym : symbols) {
		int p = prodCounter.size();
		prodCounter.push_back(prodCounter[p - 1] + sym->getProductions()->size());
	}
}

Grammer::~Grammer() {
	for (Symbol* sym : symbols) {
		delete sym;
	}
}