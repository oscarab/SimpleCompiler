#ifndef OUTPUT_H
#define OUTPUT_H

#include <fstream>
#include <vector>

class Terminator;
class Parser;

class Output {
private:
	std::vector<std::ofstream*> outs;
	bool isStep;

public:
	void add(const char*);
	std::ofstream& operator[](int);
	void setStep(bool);

	void beginningMessage(Terminator*, Parser*);
	void reductionMessage(int, int);
	void movingMessage(int);
	void sendMessage(std::string);

	~Output();
};

#endif // !OUTPUT_H
