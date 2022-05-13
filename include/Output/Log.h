#ifndef LOG

#include <fstream>
#include <string>
#include <unordered_map>
#include <iostream>

using std::string;
using std::ofstream;
using std::unordered_map;

class Log {
public:
	virtual Log* log(string) = 0;
	virtual Log* logln(string) = 0;
	virtual Log* enter() = 0;
	virtual std::ostream& origin() = 0;
};

class ConsoleLog : public Log {
public:
	static ConsoleLog* logInstance;
	Log* log(string);
	Log* logln(string);
	Log* enter();
	std::ostream& origin() { return std::cout; }
	static Log* getInstance() {
		return logInstance;
	}
private:
	ConsoleLog() {}
};

class FileLog : public Log {
public:
	static FileLog* logInstance[10];
	static unordered_map<string, int> index;
	Log* log(string);
	Log* logln(string);
	Log* enter();
	std::ostream& origin() { return *out; }
	static Log* getInstance(string name) {
		auto p = index.find(name);
		if (p == index.end()) return nullptr;
		return logInstance[p->second];
	}
	static void factoryBuild(string name, const char* file) {
		static int point = 0;
		if (index.find(name) == index.end()) {
			logInstance[point] = new FileLog(file);
			index[name] = point++;
		}
	}
private:
	ofstream* out;
	FileLog(const char* file) {
		out = new ofstream(file);
	}
};

#endif // !LOG
