#ifndef SEMANTICANALYZER_H
#define SEMANTICANALYZER_H

#include "Semantic/Quaternion.h"
#include "Semantic/SemanticAction.h"
#include <vector>
#include <unordered_map>

typedef std::string String;
using std::vector;
class Grammer;
class Symbol;

struct ValTable {
	String type;
	int offset;
	int valType;
	vector<int> dimension;
};

class IDTable {
private:
	IDTable* previous;					// 前一个符号表
	int width;							// 符号表宽度
	int position;						// 若是函数的符号表，记录函数的开始地址
	vector<String> parameters;		// 若是函数的符号表，单独记录形参

	std::unordered_map<String, ValTable> table;

	vector<IDTable*> next;			// 下张符号表
public:
	IDTable(IDTable*, int);
	void insert(String, String, int);					// 插入变量
	void insertProcess(String, String, int, bool);		// 插入函数
	void insertArray(String, String, vector<int>&, int, int);	// 插入数组
	void addNext(IDTable*);								// 添加下张符号表
	int find(String, bool);								// 寻找变量
	int findProcess(String);							// 寻找函数
	int findProcessPosition(String);					// 寻找函数位置
	int findArray(String, int);
	int getWidth();
	String getType(String);
	IDTable* getNext(int);
	IDTable* getPrevious();
	void addParameter(String);
	bool checkParameters(String);

	~IDTable();
};

class SemanticAnalyzer {
private:
	vector<vector<SemanticAction>> semanticActions;				// 每个产生式的语义动作
	std::unordered_map<String, String> symbolDistribute;		// 每个文法符号对应的属性

	int newTempCount;

	vector<Symbol*> stateStack;
	Symbol* reductionResult;
	int reductionCount;
	IDTable* nowTable;											// 符号表
	vector<Quaternion> intermediateCode;					// 中间代码
public:
	SemanticAnalyzer() : nowTable(NULL), newTempCount(0) { }

	void generateSemanticAction(Grammer*, const char*);			// 生成语义动作
	void distributeAttributeSymbols(const char*);				// 分配属性
	Symbol* createAttributeSymbol(Symbol*);						// 创建带属性的文法符号

	Symbol* getSymbolFromStack(int index);
	void moveIn(Symbol*);
	void reduce(Symbol*, int, int);
	void outputIntermediateCode();

	vector<Quaternion>& getIntermediateCode();
	unsigned int getGlobalSize();


	/*************语义动作的实现*************/
	void mktable();
	void bktable();
	void enter(String, String, int);
	void enterproc(String, String, String, String);
	String newtemp();
	String lookup(String);
	void lookupproc(String, String);
	void checkmain();
	void notlookup(String);
	void backpatch(int, String);
	void emite(String, String, String, String);
	int nextstat(int);
	String lookuptype(String);
	void checktype(String, String);
	void addpara(String);
	void enterarray(String, String, vector<int>&, int, int);
	String computearr(String, int);

	~SemanticAnalyzer();
};

#endif // !SEMANTICANALYZER_H