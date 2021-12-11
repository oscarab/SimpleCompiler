#include "Grammer.h"
#include <iostream>
#include <fstream>
#include <unordered_map>

extern const std::unordered_map<std::string, Token> tokenConvert;

// �Ƴ����ս���ļ�����
String removeBrackets(String& str) {
	return str.substr(1, str.size() - 2);
}

/**
 * @brief ����ɿյķ��ս��
*/
void Grammer::solveCanEmpty() {
	int sym_size;

	do {
		sym_size = canEmpty.size();
		// ÿ�������ս��
		for (int i = 0; i < leftSymbols.size(); i++) {
			NonTerminator* non_terminator = static_cast<NonTerminator*>(leftSymbols[i]);
			std::vector<SymbolChain>* product = non_terminator->getProductions();
			int prod_size = product->size();

			// ��Ӧÿ������ʽ
			for (int j = 0; j < prod_size; j++) {
				Symbol* sym = (*product)[j][0];
				
				// ����ʽ���п�
				if (sym->isEnd() && static_cast<Terminator*>(sym)->getToken().isEmpty()) {	
					// ����ɿշ���
					canEmpty[leftSymbols[i]] = true;
				}
				else {
					// ���������ԵĿ�
					int len = (*product)[j].size();

					// ��������ʽ��ÿ������
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
 * @brief �������з��ս����First����
*/
void Grammer::solveFirst() {
	bool flag = true;	// ��¼�Ƿ����ɹ�

	while (flag) {
		flag = false;

		for (int i = 0; i < leftSymbols.size(); i++) {	// ÿ�������ս��
			NonTerminator* non_terminator = static_cast<NonTerminator*>(leftSymbols[i]);
			int size = (*non_terminator->getProductions()).size();

			for (int j = 0; j < size; j++) {	// ��Ӧÿ������ʽ
				SymbolChain& product = (*non_terminator->getProductions())[j];
				int len = product.size();

				for (int k = 0; k < len; k++) {
					Symbol* p = product[k];

					if (p->isEnd()) {	// �����ս��
						// ֱ�Ӽӵ�first�����
						if (!static_cast<Terminator*>(p)->getToken().isEmpty()) {	// �����ǿ�
							if (firstSet[leftSymbols[i]].insert(p).second == true) flag = true;	// �ɹ�����
						}
						break;
					}
					else {	// ���Ƿ��ս��
						if (firstSet.find(p) != firstSet.end()) {	// ���ж�Ӧ��First��
							// ��First�������
							for (auto iter = firstSet[p].begin(); iter != firstSet[p].end(); ++iter) {
								if (firstSet[leftSymbols[i]].insert(*iter).second == true) flag = true;
							}
						}
						if (canEmpty.count(p)) { // �����ս���ɿ�
							p++;	// ����һ������
						}
						else {		// ���ɿ�
							break;
						}
					}
				}
			}
		}
	}
}

/**
 * @brief ������Ŵ���FIRST��
 * @param symbolStr ���Ŵ�
*/
void Grammer::solveFirst(SymbolChain& symbolStr) {
	SymbolChain temp(symbolStr);
	symbolStr.clear();

	// ����������
	int len = temp.size();
	for (int i = 0; i < len; i++) {
		// ��;�����ս�����������
		if (temp[i]->isEnd()) {
			symbolStr.push_back(temp[i]);
			break;
		}

		std::set<Symbol*>::iterator begin = (firstSet[temp[i]]).begin();
		std::set<Symbol*>::iterator end = (firstSet[temp[i]]).end();
		for (auto i = begin; i != end; ++i) {
			symbolStr.push_back(*i);
		}

		// ���÷��ս�����ɿ���������
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
 * @brief �����µ�Symbol
 * @param str ���ŵ�����
 * @param product ������Ϊ�󲿵Ĳ���ʽ
 * @return �½���Symbol
*/
Symbol* Grammer::insertSymbol(String& str, SymbolChain* product) {
	Symbol* symbol;

	if (strMapTable.find(str) == strMapTable.end()) {
		// �����·���
		if (tokenConvert.count(str)) {
			symbol = new Terminator((*tokenConvert.find(str)).second);
		}
		else {
			symbol = new NonTerminator(str);
		}

		// ���¸��ֱ�
		symbols.push_back(symbol);
		strMapTable[str] = symbols.size() - 1;
		symMapTable[symbol] = symbols.size() - 1;
	}
	else {
		// �ҵ��Ѿ����ڵķ���
		symbol = symbols[strMapTable[str]];
	}

	if (product != NULL) {
		product->push_back(symbol);
	}

	return symbol;
}

/**
 * @brief �ķ��Ĺ��캯��
 * @param filename �ķ��ļ���
*/
Grammer::Grammer(const char* filename) {
	// ��ȡ�������ķ��ļ� Grammer.txt
	std::ifstream grammerFile(filename);
	if (!grammerFile.is_open()) {
		std::cout << "fail to open grammer file!" << std::endl;
		exit(0);
	}

	String line;
	while (getline(grammerFile, line)) {	// ��ÿһ��
		// ��¼�����ս��
		int pos = line.find("::=");
		String str = line.substr(1, pos - 2);
		Symbol* symbol = insertSymbol(str, NULL);
		NonTerminator* non_terminator = static_cast<NonTerminator*>(symbol);
		leftSymbols.push_back(symbol);
		prodMapTable[symbol] = leftSymbols.size() - 1;

		// ��¼�Ҳ����
		int pos1 = pos + 3, pos2 = pos1 + 1;
		SymbolChain production;			// �洢����ʽ
		while (pos1 < line.size()) {
			if (line[pos1] == '<') {	// �������ս��
				while (line[pos2] != '>') pos2++;
				str = line.substr(pos1 + 1, pos2 - pos1 - 1);

				insertSymbol(str, &production);
				pos1 = pos2 + 1;
				pos2 = pos1 + 1;
			}
			else if(line[pos1] == '"') {	// �����ս��
				while (line[pos2] != '"') pos2++;
				str = line.substr(pos1 + 1, pos2 - pos1 - 1);

				insertSymbol(str, &production);
				pos1 = pos2 + 1;
				pos2 = pos1 + 1;
			}
			else if (line[pos1] == '|') {
				pos1++;
				pos2++;
				non_terminator->insertProduction(production);	// ���뵱ǰ����ʽ��׼����ȡ��һ������ʽ
			}
		}
		non_terminator->insertProduction(production);	// ĩβ���뵱ǰ����ʽ
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