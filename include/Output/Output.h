#ifndef OUTPUT_H
#define OUTPUT_H

#include <fstream>
#include <vector>

using std::string;

class Terminator;
class Parser;
class Log;

class Output {
private:
	static Output* output;
	Log* console;
	Log* file;
	bool isStep;
	Output();
public:
	static Output* getInstance() {
		return output;
	}

	void setStep(bool);
	void beginningMessage(Terminator*, Parser*);
	void reductionMessage(int, int);
	void movingMessage(int);
	void sendMessage(string);
};

#endif // !OUTPUT_H
