#ifndef SEMANTICANALYZER_H
#define SEMANTICANALYZER_H

#include "Semantic/Quaternion.h"
#include "Semantic/SemanticAction.h"
#include <vector>
#include <unordered_map>

typedef std::string String;
class Grammer;
class Symbol;

struct ValTable {
	String type;
	int offset;
	bool isFunction;
};

class IDTable {
private:
	IDTable* previous;					// ǰһ�����ű�
	int width;							// ���ű���
	int position;						// ���Ǻ����ķ��ű���¼�����Ŀ�ʼ��ַ
	std::vector<String> parameters;		// ���Ǻ����ķ��ű�������¼�β�

	std::unordered_map<String, ValTable> table;

	std::vector<IDTable*> next;			// ���ŷ��ű�
public:
	IDTable(IDTable*, int);
	void insert(String, String, int);					// �������
	void insertProcess(String, String, int, bool);		// ���뺯��
	void addNext(IDTable*);								// ������ŷ��ű�
	int find(String, bool);								// Ѱ�ұ���
	int findProcess(String);							// Ѱ�Һ���
	int findProcessPosition(String);					// Ѱ�Һ���λ��
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
	SemanticAnalyzer() : nowTable(NULL), newTempCount(0), out(NULL) { }

	void setOutStream(std::ostream&);							// ������Ϣ������ļ�
	void generateSemanticAction(Grammer*, const char*);			// �������嶯��
	void distributeAttributeSymbols(const char*);				// ��������
	Symbol* createAttributeSymbol(Symbol*);						// ���������Ե��ķ�����

	Symbol* getSymbolFromStack(int index);
	void moveIn(Symbol*);
	void reduce(Symbol*, int, int);
	void outputIntermediateCode(std::ostream&);

	std::vector<Quaternion>& getIntermediateCode();


	/*************���嶯����ʵ��*************/
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

	~SemanticAnalyzer();
};

#endif // !SEMANTICANALYZER_H