#ifndef GENERATOR

#include <vector>
#include <string>
#include <unordered_set>
#include "Semantic/Quaternion.h"

using std::vector;
using std::string;
using std::unordered_set;

struct Register {
	string reg;
	Variable var;
};

class Generator {
private:
	vector<string> assembly;
	vector<Register> free;
	vector<Register> occupy;
	string label;
public:
	void init(unsigned int);
	void freeAll();
	string getVariableAddress(int);
	string findFreeRegister(Variable&);			// Ѱ��һ�����еļĴ���
	string findValue(Variable&, bool=true);		// Ѱ�ұ������ڵļĴ�������Ϊ����Ҳ����Ѱ�ң�
	void addInstruction(const string&);
	void generateBatch(vector<Quaternion>&, unordered_set<string>&);

	void out();
};

#endif // !GENERATOR
