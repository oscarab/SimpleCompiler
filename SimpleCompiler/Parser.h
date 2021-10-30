#ifndef PARSER_H
#define PARSER_H

#include "Machine.h"

struct Action {
	bool action;		// 是否是ACTION
	bool reduction;		// 是否是归约
	int go;
};

typedef std::vector<std::unordered_map<Symbol, Action>> ParserTable;

class Parser {
	Machine machine;
	ParserTable table;		// 分析表
};

#endif
