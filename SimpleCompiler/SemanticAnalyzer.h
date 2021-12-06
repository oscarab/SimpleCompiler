#ifndef SEMANTICANALYZER_H
#define SEMANTICANALYZER_H

#include "Grammer.h"
#include <vector>

class SemanticAnalyzer {
private:
	SymbolChain leftSymbol;
public:
	void update(Grammer*);
	void generateSemanticAction(const char*);
	void insertSymbol(Symbol*);
};

#endif // !SEMANTICANALYZER_H
