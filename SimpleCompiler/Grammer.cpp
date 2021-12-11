#include "Grammer.h"
#include <iostream>
#include <fstream>
#include <unordered_map>

extern const std::unordered_map<std::string, Token> tokenConvert;

// 移除非终结符的尖括号
String removeBrackets(String& str) {
	return str.substr(1, str.size() - 2);
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
			NonTerminator* non_terminator = static_cast<NonTerminator*>(leftSymbols[i]);
			std::vector<SymbolChain>* product = non_terminator->getProductions();
			int prod_size = product->size();

			// 对应每个产生式
			for (int j = 0; j < prod_size; j++) {
				Symbol* sym = (*product)[j][0];
				
				// 产生式中有空
				if (sym->isEnd() && static_cast<Terminator*>(sym)->getToken().isEmpty()) {	
					// 插入可空符表
					canEmpty[leftSymbols[i]] = true;
				}
				else {
					// 不存在明显的空
					int len = (*product)[j].size();

					// 遍历产生式中每个符号
					for (int k = 0; k < len; k++) {
						if (!canEmpty.count((*product)[j][k])) {
							break;
						}
						if (k == len - 1) {
							canEmpty[leftSymbols[i]] = true;
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
			NonTerminator* non_terminator = static_cast<NonTerminator*>(leftSymbols[i]);
			int size = (*non_terminator->getProductions()).size();

			for (int j = 0; j < size; j++) {	// 对应每个产生式
				SymbolChain& product = (*non_terminator->getProductions())[j];
				int len = product.size();

				for (int k = 0; k < len; k++) {
					Symbol* p = product[k];

					if (p->isEnd()) {	// 若是终结符
						// 直接加到first集最后
						if (!static_cast<Terminator*>(p)->getToken().isEmpty()) {	// 不考虑空
							if (firstSet[leftSymbols[i]].insert(p).second == true) flag = true;	// 成功插入
						}
						break;
					}
					else {	// 若是非终结符
						if (firstSet.find(p) != firstSet.end()) {	// 若有对应的First集
							// 将First逐个加入
							for (auto iter = firstSet[p].begin(); iter != firstSet[p].end(); ++iter) {
								if (firstSet[leftSymbols[i]].insert(*iter).second == true) flag = true;
							}
						}
						if (canEmpty.count(p)) { // 若非终结符可空
							p++;	// 看下一个符号
						}
						else {		// 不可空
							break;
						}
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
void Grammer::solveFirst(SymbolChain& symbolStr) {
	SymbolChain temp(symbolStr);
	symbolStr.clear();

	// 遍历整个串
	int len = temp.size();
	for (int i = 0; i < len; i++) {
		// 中途遇到终结符，计算结束
		if (temp[i]->isEnd()) {
			symbolStr.push_back(temp[i]);
			break;
		}

		std::set<Symbol*>::iterator begin = (firstSet[temp[i]]).begin();
		std::set<Symbol*>::iterator end = (firstSet[temp[i]]).end();
		for (auto i = begin; i != end; ++i) {
			symbolStr.push_back(*i);
		}

		// 若该非终结符不可空则计算结束
		if (canEmpty.find(temp[i]) == canEmpty.end()) {
			break;
		}
	}
}

Symbol* Grammer::getSymbol(String str) {
	return symbols[strMapTable[str]];
}

SymbolChain* Grammer::getSymbols() {
	return &symbols;
}

int Grammer::getProductionCount(Symbol* symbol) {
	return productCounter[prodMapTable[symbol]];
}

/**
 * @brief 插入新的Symbol
 * @param str 符号的名字
 * @param product 符号作为左部的产生式
 * @return 新建的Symbol
*/
Symbol* Grammer::insertSymbol(String& str, SymbolChain* product) {
	Symbol* symbol;

	if (strMapTable.find(str) == strMapTable.end()) {
		// 创建新符号
		if (tokenConvert.count(str)) {
			symbol = new Terminator((*tokenConvert.find(str)).second);
		}
		else {
			symbol = new NonTerminator(str);
		}

		// 更新各种表
		symbols.push_back(symbol);
		strMapTable[str] = symbols.size() - 1;
		symMapTable[symbol] = symbols.size() - 1;
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
	if (!grammerFile.is_open()) {
		std::cout << "fail to open grammer file!" << std::endl;
		exit(0);
	}

	String line;
	while (getline(grammerFile, line)) {	// 读每一行
		// 记录左侧非终结符
		int pos = line.find("::=");
		String str = line.substr(1, pos - 2);
		Symbol* symbol = insertSymbol(str, NULL);
		NonTerminator* non_terminator = static_cast<NonTerminator*>(symbol);
		leftSymbols.push_back(symbol);
		prodMapTable[symbol] = leftSymbols.size() - 1;

		// 记录右侧符号
		int pos1 = pos + 3, pos2 = pos1 + 1;
		SymbolChain production;			// 存储产生式
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
				non_terminator->insertProduction(production);	// 插入当前产生式，准备读取下一条产生式
			}
		}
		non_terminator->insertProduction(production);	// 末尾插入当前产生式
	}
	grammerFile.close();

	String end = String("#");
	insertSymbol(end, NULL);

	productCounter.push_back(0);
	for (Symbol* sym : leftSymbols) {
		int p = productCounter.size();

		NonTerminator* non_terminator = static_cast<NonTerminator*>(sym);
		productCounter.push_back(productCounter[p - 1] + non_terminator->getProductions()->size());
	}
}

Grammer::~Grammer() {
	for (Symbol* sym : symbols) {
		delete sym;
	}
}