#ifndef GENERATOR

#include <vector>
#include <string>
#include "Semantic/Quaternion.h"

using std::vector;
using std::string;

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
	void init();
	void freeAll();
	string findFreeRegister(Variable&);			// Ѱ��һ�����еļĴ���
	string findValue(Variable&, bool=true);		// Ѱ�ұ������ڵļĴ�������Ϊ����Ҳ����Ѱ�ң�
	void addInstruction(const string&);
	void generateBatch(vector<Quaternion>&);

	void out();
};

#endif // !GENERATOR
