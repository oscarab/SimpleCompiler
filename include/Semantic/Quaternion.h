#ifndef QUATERNION_H
#define QUATERNION_H

#include <string>
#include <iostream>

struct Variable {
	std::string name;
	int address;
};

class Quaternion {
private:
	std::string label;
	std::string operate;
	Variable parameter1;
	Variable parameter2;
	Variable result;
public:
	Quaternion(std::string, Variable, Variable, Variable);

	void setLabel(std::string);
	std::string getLabel();
	void setResult(Variable);
	bool isJump();
	void output(std::ostream&);

	std::string getOperator() {
		return operate;
	}

	Variable getParameter(int no) {
		return no == 1 ? parameter1 : parameter2;
	}

	Variable getResult() {
		return result;
	}

	int getAddress() {
		return std::stoi(result.name);
	}
};

#endif // !QUATERNION_H

