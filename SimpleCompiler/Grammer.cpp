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
 * @brief ����һ���²���ʽ
 * @param psymbol ����ʽ�Ҳ��ķ��Ŵ�
*/
void Symbol::insertProduction(PSymbol& psymbol) {
	production.push_back(psymbol);	// ���²���ʽ
	psymbol.clear();				//׼��������һ������ʽ
}

/**
 * @brief ���
 * @param out ���Դ
 * @param level ����
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


// �Ƴ����ս���ļ�����
String removeBrackets(String& str) {
	return str.substr(1, str.size() - 2);
}

void Grammer::getProduction(int numSymbol, int numProduction, PSymbol& psymbol) {
	// ��ȡ��numSymbol�����ս������numProduction������ʽ
	PSymbol& product = (*symbols[numSymbol]->getProductions())[numProduction];
	psymbol.insert(psymbol.end(), product.begin(), product.end());
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
			std::vector<PSymbol>* prod = leftSymbols[i]->getProductions();
			int prod_size = prod->size();

			// ��Ӧÿ������ʽ
			for (int j = 0; j < prod_size; j++) {
				if ((*prod)[j][0]->getToken() == Token(TokenType::EPSILON, TokenAttribute::None)) {	// ����ʽ���п�
					// ����ɿշ���
					canEmpty[*leftSymbols[i]] = true;
				}
				else {
					// ���������ԵĿ�
					int len = (*prod)[j].size();

					// ��������ʽ��ÿ������
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
 * @brief �������з��ս����First����
*/
void Grammer::solveFirst() {
	bool flag = true;	// ��¼�Ƿ����ɹ�

	while (flag) {
		flag = false;

		for (int i = 0; i < leftSymbols.size(); i++) {	// ÿ�������ս��
			for (int j = 0; j < (*leftSymbols[i]->getProductions()).size(); j++) {	// ��Ӧÿ������ʽ
				PSymbol& prod = (*leftSymbols[i]->getProductions())[j];
				int len = prod.size();

				for (int k = 0; k < len; k++) {
					Symbol* p = prod[k];

					if (p->isEnd()) {	// �����ս��
					// ֱ�Ӽӵ�first�����
						if (p->getToken().getType() != TokenType::EPSILON) {	// �����ǿ�
							if (firstSet[*leftSymbols[i]].insert(p).second == true) flag = true;	// �ɹ�����
						}
						break;
					}
					else {	// ���Ƿ��ս��
						if (firstSet.find(*p) != firstSet.end()) {	// ���ж�Ӧ��First��
							// ��First�������
							for (auto iter = firstSet[*p].begin(); iter != firstSet[*p].end(); ++iter) {
								if (firstSet[*leftSymbols[i]].insert(*iter).second == true) flag = true;
							}
						}
						if (canEmpty.count(*p))	// �����ս���ɿ�
							p++;	// ����һ������
						else	// ���ɿ�
							break;
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
void Grammer::solveFirst(PSymbol& symbolStr) {
	PSymbol temp(symbolStr);
	symbolStr.clear();

	// ����������
	int len = temp.size();
	for (int i = 0; i < len; i++) {
		// ��;�����ս�����������
		if (temp[i]->isEnd()) {
			symbolStr.push_back(temp[i]);
			break;
		}

		std::set<Symbol*>::iterator begin = (firstSet[*temp[i]]).begin();
		std::set<Symbol*>::iterator end = (firstSet[*temp[i]]).end();
		for (auto i = begin; i != end; ++i) {
			symbolStr.push_back(*i);
		}

		// ���÷��ս�����ɿ���������
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
		// �����·���
		if (tokenConvert.count(str)) {
			symbol = new Symbol((*tokenConvert.find(str)).second);
		}
		else {
			symbol = new Symbol(str);
		}

		// ���¸��ֱ�
		symbols.push_back(symbol);
		strMapTable[str] = symbols.size() - 1;
		symMapTable[*symbol] = symbols.size() - 1;
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
	if (!grammerFile.is_open())
		std::cout << "���ķ��ļ�ʧ�ܣ�" << std::endl;

	String line;
	while (getline(grammerFile, line)) {	// ��ÿһ��
		// ��¼�����ս��
		int pos = line.find("::=");
		String str = line.substr(1, pos - 2);
		Symbol* symbol = insertSymbol(str, NULL);
		leftSymbols.push_back(symbol);

		// ��¼�Ҳ����
		int pos1 = pos + 3, pos2 = pos1 + 1;
		PSymbol production;	// �洢����ʽ
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
				symbol->insertProduction(production);	// ���뵱ǰ����ʽ��׼����ȡ��һ������ʽ
			}
		}
		symbol->insertProduction(production);	// ĩβ���뵱ǰ����ʽ
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