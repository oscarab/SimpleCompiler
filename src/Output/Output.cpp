#include "Output/Output.h"
#include "Parser/Symbol.h"
#include "Parser/Parser.h"
#include <sstream>
#include <cstring>
#include <iostream>
#include <conio.h>

using std::string;
using std::ofstream;

void Output::setStep(bool flag) {
	isStep = flag;
}

void Output::add(const char* file) {
	outs.push_back(new ofstream(file));
}

std::ofstream& Output::operator[](int idx) {
	return *outs[idx];
}

void Output::beginningMessage(Terminator* terminator, Parser* parser) {
	if (isStep) {
		system("cls");
		parser->writeStack(std::cout);
		std::cout << std::endl << "Now at: " << std::endl;
		terminator->write(std::cout, 0);
		std::cout << std::endl << "Action: " << std::endl;
	}
	parser->writeStack(*outs[1]);
	*outs[1] << std::endl << "Now at: " << std::endl;
	terminator->write(*outs[1], 0);
	*outs[1] << std::endl << "Action: " << std::endl;
}

void Output::reductionMessage(int reduce_cnt, int dest) {
	std::stringstream str;
	str << "Reduce the " << reduce_cnt << " characters at the top of the stack" << std::endl;
	str << "Goto state " << dest << std::endl;

	if (isStep) {
		std::cout << str.str();
		_getch();
	}
	*outs[1] << str.str();
}

void Output::movingMessage(int dest) {
	std::stringstream str;
	str  << "Push the current symbol onto the symbol stack" << std::endl;
	str << "Goto state " << dest << std::endl;

	if (isStep) {
		std::cout << str.str();
		_getch();
	}
	*outs[1] << str.str();
}

void Output::sendMessage(String text) {
	std::cout << text << std::endl;
	*outs[1] << text << std::endl;
}

Output::~Output() {
	for (ofstream* fout : outs) {
		fout->close();
		delete fout;
	}
}