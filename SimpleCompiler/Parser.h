#ifndef PARSER_H
#define PARSER_H

#include "Machine.h"

struct Action {
	bool action;		// 是否是ACTION
	bool reduction;		// 是否是归约
	bool accept;		// 接受状态
	int go;				// 转移状态
	Production prod;	// 归约用到的产生式

	Action() : prod(NULL, 0, 0) { }
};

typedef std::vector<std::unordered_map<Symbol, Action>> ParserTable;

class Parser {
private:

	Machine machine;		// 自动机
	ParserTable table;		// 分析表

	std::vector<int> stateStack;		// 状态栈
	std::vector<Symbol*> symbolStack;	// 符号栈

public:
	Parser(const char*);

	void createTable();
	void analysis(const char*);
};

#endif
