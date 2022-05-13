#include "Output/Log.h"
#include <iostream>

ConsoleLog*  ConsoleLog::logInstance = new ConsoleLog();
FileLog* FileLog::logInstance[10];
unordered_map<string, int> FileLog::index;

Log* ConsoleLog::log(string msg) {
	std::cout << msg;
	return this;
}

Log* ConsoleLog::logln(string msg) {
	std::cout << msg << std::endl;
	return this;
}

Log* ConsoleLog::enter() {
	std::cout << std::endl;
	return this;
}

Log* FileLog::log(string msg) {
	*out << msg;
	return this;
}

Log* FileLog::logln(string msg) {
	*out << msg << std::endl;
	return this;
}

Log* FileLog::enter() {
	*out << std::endl;
	return this;
}