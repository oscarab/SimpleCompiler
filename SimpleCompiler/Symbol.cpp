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
 * @brief 插入一条新产生式
 * @param psymbol 产生式右部的符号串
*/
void NonTerminator::insertProduction(SymbolChain& psymbol) {
	production.push_back(psymbol);	// 更新产生式
	psymbol.clear();				//准备读入下一条产生式
}

/**
 * @brief 非终结符输出
 * @param out 输出源
 * @param level 缩进
*/
void NonTerminator::write(std::ostream& out, int level) {
	tab(out, level);
	out << "\"type\": " << "\"NonTerminator\"" << "," << std::endl;
	tab(out, level);
	out << "\"attribute\": \"" << name << "\"," << std::endl;
}

/**
 * @brief 终结符输出
 * @param out 输出源
 * @param level 缩进
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