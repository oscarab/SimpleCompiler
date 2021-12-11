#ifndef SEMANTICANALYZER_H
#define SEMANTICANALYZER_H

#include "Quaternion.h"
#include "SemanticAction.h"
#include <vector>
#include <unordered_map>

typedef std::string String;
class Grammer;
class Symbol;

struct ValTable {
	String type;
	int offset;
};

class IDTable {
private:
	IDTable* previous;
	int width;

	std::unordered_map<String, ValTable> table;

	std::vector<IDTable*> next;
public:
	IDTable(IDTable*);
	void insert(String name, String type, int w);
	void addNext(IDTable*);
	int find(String name);
	IDTable* getPrevious();
};

class SemanticAnalyzer {
private:
	std::vector<std::vector<SemanticAction>> semanticActions;	// 每个产生式的语义动作
	std::unordered_map<String, String> symbolDistribute;		// 每个文法符号对应的属性

	int newTempCount;

	std::vector<Symbol*> stateStack;
	Symbol* reductionResult;
	int reductionCount;
	IDTable* nowTable;											// 符号表
	std::vector<Quaternion> intermediateCode;					// 中间代码
public:
	void generateSemanticAction(Grammer*, const char*);			// 生成语义动作
	void distributeAttributeSymbols(const char*);				// 分配属性
	Symbol* createAttributeSymbol(Symbol*);						// 创建带属性的文法符号

	Symbol* getSymbolFromStack(int index);
	void moveIn(Symbol*);
	void reduce(Symbol*, int, int);
	void outputIntermediateCode();


	/*************语义动作的实现*************/
	void mktable();
	void bktable();
	void enter(String, String, int);
	String newtemp();
	String lookup(String);
	void notlookup(String);
	void backpatch(int, String);
	void emite(String, String, String, String);
	int nextstat(int);
};

#endif // !SEMANTICANALYZER_H