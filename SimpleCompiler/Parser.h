#ifndef PARSER_H
#define PARSER_H

#include "Machine.h"

struct Action {
	bool action;		// �Ƿ���ACTION
	bool reduction;		// �Ƿ��ǹ�Լ
	int go;
};

typedef std::vector<std::unordered_map<Symbol, Action>> ParserTable;

class Parser {
	Machine machine;
	ParserTable table;		// ������
};

#endif
