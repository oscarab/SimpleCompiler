#include "Token.h"
#include "Symbol.h"

extern void tab(std::ostream& out, int level);

Terminator::Terminator(Token _token) : token(_token) {
}

Token Terminator::getToken() const {
	return token;
}

bool Terminator::isEnd() const {
	return true;
}

NonTerminator::NonTerminator(String str) {
	name = str;
}

String NonTerminator::getName() const {
	return name;
}

bool NonTerminator::isEnd() const {
	return false;
}

std::vector<SymbolChain>* NonTerminator::getProductions() {
	return &production;
}

/**
 * @brief ����һ���²���ʽ
 * @param psymbol ����ʽ�Ҳ��ķ��Ŵ�
*/
void NonTerminator::insertProduction(SymbolChain& psymbol) {
	production.push_back(psymbol);	// ���²���ʽ
	psymbol.clear();				//׼��������һ������ʽ
}

/**
 * @brief ���ս�����
 * @param out ���Դ
 * @param level ����
*/
void NonTerminator::write(std::ostream& out, int level) {
	tab(out, level);
	out << "\"type\": " << "\"NonTerminator\"" << "," << std::endl;
	tab(out, level);
	out << "\"attribute\": \"" << name << "\"," << std::endl;
}

/**
 * @brief �ս�����
 * @param out ���Դ
 * @param level ����
*/
void Terminator::write(std::ostream& out, int level) {
	token.write(out, level, false);
}

bool Symbol::operator==(const Symbol& symbol) const {
	bool end = isEnd();

	if (symbol.isEnd() != end) return false;
	if (end) {
		Terminator* tor1 = static_cast<Terminator*>(const_cast<Symbol*>(this));
		Terminator* tor2 = static_cast<Terminator*>(const_cast<Symbol*>(&symbol));
		return tor1->getToken() == tor2->getToken();
	}
	else {
		NonTerminator* nontor1 = static_cast<NonTerminator*>(const_cast<Symbol*>(this));
		NonTerminator* nontor2 = static_cast<NonTerminator*>(const_cast<Symbol*>(&symbol));
		return nontor1->getName() == nontor2->getName();
	}
}