#ifndef OUTPUT_H
#define OUTPUT_H

#include <fstream>
#include <vector>
#include <cstring>

class Output {
private:
	std::vector<std::ofstream*> outs;

public:
	void add(const char*);
	std::ofstream& operator[](int);
	~Output();
};

#endif // !OUTPUT_H
