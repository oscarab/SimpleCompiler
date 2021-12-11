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
	std::vector<std::vector<SemanticAction>> semanticActions;	// ÿ������ʽ�����嶯��
	std::unordered_map<String, String> symbolDistribute;		// ÿ���ķ����Ŷ�Ӧ������

	int newTempCount;

	std::vector<Symbol*> stateStack;
	Symbol* reductionResult;
	int reductionCount;
	IDTable* nowTable;											// ���ű�
	std::vector<Quaternion> intermediateCode;					// �м����
public:
	void generateSemanticAction(Grammer*, const char*);			// �������嶯��
	void distributeAttributeSymbols(const char*);				// ��������
	Symbol* createAttributeSymbol(Symbol*);						// ���������Ե��ķ�����

	Symbol* getSymbolFromStack(int index);
	void moveIn(Symbol*);
	void reduce(Symbol*, int, int);
	void outputIntermediateCode();


	/*************���嶯����ʵ��*************/
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