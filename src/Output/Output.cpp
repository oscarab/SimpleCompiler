#include "Output/Output.h"
#include "Parser/Symbol.h"
#include "Parser/Parser.h"
#include "Output/Log.h"
#include <sstream>
#include <cstring>
#include <iostream>
#include <conio.h>

using std::string;
using std::ofstream;

Output* Output::output = new Output();

Output::Output() {
	isStep = false;
	console = ConsoleLog::getInstance();
	file = nullptr;
}

void Output::setStep(bool flag) {
	isStep = flag;
	file = FileLog::getInstance("analysis.txt");
}

void Output::beginningMessage(Terminator* terminator, Parser* parser) {
	if (isStep) {
		system("cls");
		parser->writeStack(console->origin());
		console->enter()->logln("Now at: ");
		terminator->write(console, 0);
		console->enter()->logln("Action");
	}
	parser->writeStack(file->origin());
	file->enter()->logln("Now at: ");
	terminator->write(file, 0);
	file->enter()->logln("Action");
}

void Output::reductionMessage(int reduce_cnt, int dest) {
	std::stringstream str;
	str << "Reduce the " << reduce_cnt << " characters at the top of the stack" << std::endl;
	str << "Goto state " << dest << std::endl;

	if (isStep) {
		console->log(str.str());
		_getch();
	}
	file->log(str.str());
}

void Output::movingMessage(int dest) {
	std::stringstream str;
	str  << "Push the current symbol onto the symbol stack" << std::endl;
	str << "Goto state " << dest << std::endl;

	if (isStep) {
		console->log(str.str());
		_getch();
	}
	file->log(str.str());
}

void Output::sendMessage(String text) {
	console->logln(text);
	file->logln(text);
}