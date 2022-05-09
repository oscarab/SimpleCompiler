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
	string findFreeRegister(Variable&);			// 寻找一个空闲的寄存器
	string findValue(Variable&, bool=true);		// 寻找变量所在的寄存器（若为常数也进行寻找）
	void addInstruction(const string&);
	void generateBatch(vector<Quaternion>&, unordered_set<string>&);

	void out();
};

#endif // !GENERATOR
