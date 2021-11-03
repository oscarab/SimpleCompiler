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
	production.push_back(psymbol);	// ���²���ʽ
	psymbol.clear();				//׼��������һ������ʽ
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

// �����ս���ַ���ΪToken��ʽ
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
	// ��ȡ��numSymbol�����ս������numProduction������ʽ
	PSymbol& product = (*symbols[numSymbol]->getProductions())[numProduction];
	psymbol.insert(psymbol.end(), product.begin(), product.end());
}

// ����ɿյķ��ս��
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

// �������з��ս����First����
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
	return symMapTable.find(*symbol)->second;
}

PSymbol* Grammer::getSymbols() {
	return &symbols;
}

int Grammer::getProductionCount(int index) {
	return prodCounter[index];
}

Grammer::Grammer(const char* filename) {
	// ��ȡ�������ķ��ļ� Grammer.txt
	std::ifstream grammerFile(filename);
	if (!grammerFile.is_open())
		std::cout << "���ķ��ļ�ʧ�ܣ�" << std::endl;

	String line;
	while (getline(grammerFile, line)) {	// ��ÿһ��
		// ��¼�����ս��
		int pos = line.find("::=");
		String s = line.substr(0, pos);
		Symbol* symbol;

		if (strMapTable.find(removeBrackets(s)) == strMapTable.end()) {
			symbol = new Symbol(removeBrackets(s));				// "::="�����ս�� Symbol(String)
			symbols.push_back(symbol);							// ���з���
			strMapTable[removeBrackets(s)] = symbols.size() - 1;	// �ַ������±�
			symMapTable[*symbol] = symbols.size() - 1;			// �������±�
			leftSymbols.push_back(symbol);						// �����󲿷���
		}
		else {
			symbol = symbols[strMapTable[removeBrackets(s)]];
			leftSymbols.push_back(symbol);
		}

		// ��¼�Ҳ����
		int pos1 = pos + 3, pos2 = pos1 + 1;
		PSymbol production;	// �洢����ʽ
		while (pos1 < line.size()) {
			if (line[pos1] == '<') {	// �������ս��
				while (line[pos2] != '>') pos2++;
				s = line.substr(pos1 + 1, pos2 - pos1 - 1);

				if (strMapTable.find(s) == strMapTable.end()) {	// û��������
					Symbol* _tempSymbol = new Symbol(s);
					production.push_back(_tempSymbol);	// ���µ�ǰ����ʽ
					symbols.push_back(_tempSymbol);	// ���з���
					strMapTable.insert(std::pair<String, int>(s, symbols.size() - 1));	// �ַ������±�
					symMapTable.insert(std::pair<Symbol, int>(*_tempSymbol, symbols.size() - 1));	// �������±�
				}
				else {	// ������
					production.push_back(symbols[strMapTable[s]]);
				}
				// ���ս��ֻ����Ϊ�󲿷���ʱ�����ϣ��
				pos1 = pos2 + 1;
				pos2 = pos1 + 1;
			}
			else if(line[pos1] == '"') {	// �����ս��
				while (line[pos2] != '"') pos2++;
				s = line.substr(pos1 + 1, pos2 - pos1 - 1);

				if (strMapTable.find(s) == strMapTable.end()) {	// û��������
					Symbol* _tempSymbol = new Symbol(setToken(s));
					production.push_back(_tempSymbol);	// ���µ�ǰ����ʽ
					symbols.push_back(_tempSymbol);	// ���з���
					strMapTable.insert(std::pair<String, int>(s, symbols.size() - 1));	// �ַ������±�
					symMapTable.insert(std::pair<Symbol, int>(*_tempSymbol, symbols.size() - 1));	// �������±�
				}
				else {	// ������
					production.push_back(symbols[strMapTable[s]]);
				}
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