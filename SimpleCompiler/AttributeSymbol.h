#ifndef ATTRIBUTESYMBOL_H
#define ATTRIBUTESYMBOL_H

#include "Symbol.h"

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
	int width;
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

class ControlSymbol : public NonTerminator {
private:
	int truelist;
	int falselist;
public:
	ControlSymbol(String name) : NonTerminator(name) {}
	virtual void setupReflect();
};

class EpsilonSymbol : public NonTerminator {
private:
	int quad;
	int truelist;
public:
	EpsilonSymbol(String name) : NonTerminator(name) {}
	virtual void setupReflect();
};

#endif // !ATTRIBUTESYMBOL_H

