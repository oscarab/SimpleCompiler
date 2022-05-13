#ifndef GENERATOR

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
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
	unordered_set<string> locked;
	unordered_set<string> index_handled;
	string label;
public:
	void init(unsigned int);
	void freeAll();
	void freeArrayInRegister();
	string getArrayIndexReg(Variable&);
	string getVariableAddress(int);
	string findInRegsiter(Variable&);
	string findFreeRegister(Variable&);			// 寻找一个空闲的寄存器
	string findValue(Variable&, bool=true);		// 寻找变量所在的寄存器（若为常数也进行寻找）
	void addInstruction(const string&);
	void generateBatch(vector<Quaternion>&, unordered_set<string>&);

	void out();
};

#endif // !GENERATOR
