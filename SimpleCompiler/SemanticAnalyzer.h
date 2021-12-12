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
	std::vector<std::vector<SemanticAction>> semanticActions;	// ÿ������ʽ�����嶯��
	std::unordered_map<String, String> symbolDistribute;		// ÿ���ķ����Ŷ�Ӧ������
	std::ostream* out;

	int newTempCount;

	std::vector<Symbol*> stateStack;
	Symbol* reductionResult;
	int reductionCount;
	IDTable* nowTable;											// ���ű�
	std::vector<Quaternion> intermediateCode;					// �м����
public:
	void setOutStream(std::ostream&);							// ������Ϣ������ļ�
	void generateSemanticAction(Grammer*, const char*);			// �������嶯��
	void distributeAttributeSymbols(const char*);				// ��������
	Symbol* createAttributeSymbol(Symbol*);						// ���������Ե��ķ�����

	Symbol* getSymbolFromStack(int index);
	void moveIn(Symbol*);
	void reduce(Symbol*, int, int);
	void outputIntermediateCode(std::ostream&);


	/*************���嶯����ʵ��*************/
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