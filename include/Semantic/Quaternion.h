#ifndef QUATERNION_H
#define QUATERNION_H

#include <string>
#include <iostream>

struct Variable {
	std::string name;
	int offset;
};

class Quaternion {
private:
	std::string operate;
	std::string arg1;
	std::string arg2;
	std::string result;
public:
	Quaternion(std::string, std::string, std::string, std::string);

	void setResult(std::string);
	void output(std::ostream&);

	std::string getOperator() {
		return operate;
	}

	std::string getArg1() {
		return arg1;
	}

	std::string getArg2() {
		return arg2;
	}

	std::string getResult() {
		return result;
	}

	int getAddress() {
		return std::stoi(result);
	}
};

#endif // !QUATERNION_H

