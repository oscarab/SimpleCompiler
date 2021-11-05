#include "Grammer.h"
#include <iostream>
#include <fstream>
#include <unordered_map>

extern const std::unordered_map<std::string, Token> tokenConvert;
extern void tab(std::ostream& out, int level);

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

/**
 * @brief 插入一条新产生式
 * @param psymbol 产生式右部的符号串
*/
void Symbol::insertProduction(PSymbol& psymbol) {
	production.push_back(psymbol);	// 更新产生式
	psymbol.clear();				//准备读入下一条产生式
}

/**
 * @brief 输出
 * @param out 输出源
 * @param level 缩进
*/
void Symbol::write(std::ostream& out, int level) {
	if (!end) {
		tab(out, level);
		out << "\"type\": " << "\"NonTerminator\"" << "," << std::endl;
		tab(out, level);
		out << "\"attribute\": \"" << name << "\"," << std::endl;
	}
	else {
		token.write(out, level, false);
	}
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

void Grammer::getProduction(int numSymbol, int numProduction, PSymbol& psymbol) {
	// 获取第numSymbol个非终结符，第numProduction个产生式
	PSymbol& product = (*symbols[numSymbol]->getProductions())[numProduction];
	psymbol.insert(psymbol.end(), product.begin(), product.end());
}

/**
 * @brief 计算可空的非终结符
*/
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

/**
 * @brief 计算所有非终结符的First集合
*/
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

/**
 * @brief 计算符号串的FIRST集
 * @param symbolStr 符号串
*/
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
	return symMapTable[*symbol];
}

Symbol* Grammer::getSymbol(Symbol* symbol) {
	return symbols[symMapTable[*symbol]];
}

Symbol* Grammer::getSymbol(String str) {
	return symbols[strMapTable[str]];
}

PSymbol* Grammer::getSymbols() {
	return &symbols;
}

int Grammer::getProductionCount(int index) {
	return prodCounter[index];
}

Symbol* Grammer::insertSymbol(String& str, PSymbol* product) {
	Symbol* symbol;

	if (strMapTable.find(str) == strMapTable.end()) {
		// 创建新符号
		if (tokenConvert.count(str)) {
			symbol = new Symbol((*tokenConvert.find(str)).second);
		}
		else {
			symbol = new Symbol(str);
		}

		// 更新各种表
		symbols.push_back(symbol);
		strMapTable[str] = symbols.size() - 1;
		symMapTable[*symbol] = symbols.size() - 1;
	}
	else {
		// 找到已经存在的符号
		symbol = symbols[strMapTable[str]];
	}

	if (product != NULL) {
		product->push_back(symbol);
	}

	return symbol;
}

/**
 * @brief 文法的构造函数
 * @param filename 文法文件名
*/
Grammer::Grammer(const char* filename) {
	// 读取并分析文法文件 Grammer.txt
	std::ifstream grammerFile(filename);
	if (!grammerFile.is_open())
		std::cout << "打开文法文件失败！" << std::endl;

	String line;
	while (getline(grammerFile, line)) {	// 读每一行
		// 记录左侧非终结符
		int pos = line.find("::=");
		String str = line.substr(1, pos - 2);
		Symbol* symbol = insertSymbol(str, NULL);
		leftSymbols.push_back(symbol);

		// 记录右侧符号
		int pos1 = pos + 3, pos2 = pos1 + 1;
		PSymbol production;	// 存储产生式
		while (pos1 < line.size()) {
			if (line[pos1] == '<') {	// 读到非终结符
				while (line[pos2] != '>') pos2++;
				str = line.substr(pos1 + 1, pos2 - pos1 - 1);

				insertSymbol(str, &production);
				pos1 = pos2 + 1;
				pos2 = pos1 + 1;
			}
			else if(line[pos1] == '"') {	// 读到终结符
				while (line[pos2] != '"') pos2++;
				str = line.substr(pos1 + 1, pos2 - pos1 - 1);

				insertSymbol(str, &production);
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

	String end = String("#");
	insertSymbol(end, NULL);

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