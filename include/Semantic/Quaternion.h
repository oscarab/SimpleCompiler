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
};

#endif // !QUATERNION_H

