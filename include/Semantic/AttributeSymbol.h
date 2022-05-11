#ifndef ATTRIBUTESYMBOL_H
#define ATTRIBUTESYMBOL_H

#include "Parser/Symbol.h"
#include <vector>

class LeafSymbol : public Terminator {
private:
	String name;
	String place;
	String type;
public:
	LeafSymbol(Token);
	virtual void setupReflect();
};

class VariableSymbol : public NonTerminator {
private:
	String place;
	String type;
	String array;
	int width;
	std::vector<int> truelist;
	std::vector<int> falselist;
public:
	VariableSymbol(String name) : NonTerminator(name) { place = ""; }
	virtual void setupReflect();
};

class OperatorSymbol : public NonTerminator {
private:
	String op;
public:
	OperatorSymbol(String name) : NonTerminator(name) {}
	virtual void setupReflect();
};

class EpsilonSymbol : public NonTerminator {
private:
	int quad;
	int nextlist;
public:
	EpsilonSymbol(String name) : NonTerminator(name) {}
	virtual void setupReflect();
};

#endif // !ATTRIBUTESYMBOL_H

