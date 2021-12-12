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
	bool isFunc;
};

class IDTable {
private:
	IDTable* previous;
	int width;

	std::unordered_map<String, ValTable> table;

	std::vector<IDTable*> next;
public:
	IDTable(IDTable*);
	void insert(String, String, int);
	void insertProc(String, String, bool);
	void addNext(IDTable*);
	int find(String);
	int findProc(String);
	IDTable* getPrevious();

	~IDTable();
};

class SemanticAnalyzer {
private:
	std::vector<std::vector<SemanticAction>> semanticActions;	// 每个产生式的语义动作
	std::unordered_map<String, String> symbolDistribute;		// 每个文法符号对应的属性
	std::ostream* out;

	int newTempCount;

	std::vector<Symbol*> stateStack;
	Symbol* reductionResult;
	int reductionCount;
	IDTable* nowTable;											// 符号表
	std::vector<Quaternion> intermediateCode;					// 中间代码
public:
	void setOutStream(std::ostream&);							// 设置信息输出的文件
	void generateSemanticAction(Grammer*, const char*);			// 生成语义动作
	void distributeAttributeSymbols(const char*);				// 分配属性
	Symbol* createAttributeSymbol(Symbol*);						// 创建带属性的文法符号

	Symbol* getSymbolFromStack(int index);
	void moveIn(Symbol*);
	void reduce(Symbol*, int, int);
	void outputIntermediateCode(std::ostream&);


	/*************语义动作的实现*************/
	void mktable();
	void bktable();
	void enter(String, String, int);
	void enterproc(String, String, String);
	String newtemp();
	String lookup(String);
	void lookupproc(String);
	void checkmain();
	void notlookup(String);
	void backpatch(int, String);
	void emite(String, String, String, String);
	int nextstat(int);

	~SemanticAnalyzer();
};

#endif // !SEMANTICANALYZER_H