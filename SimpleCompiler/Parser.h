#ifndef PARSER_H
#define PARSER_H

#include "Machine.h"

struct Action {
	bool action;		// �Ƿ���ACTION
	bool reduction;		// �Ƿ��ǹ�Լ
	bool accept;		// ����״̬
	int go;				// ת��״̬
	Production prod;	// ��Լ�õ��Ĳ���ʽ

	Action() : prod(NULL, 0, 0) { }
};

typedef std::vector<std::unordered_map<Symbol, Action>> ParserTable;

class Parser {
private:

	Machine machine;		// �Զ���
	ParserTable table;		// ������

	std::vector<int> stateStack;		// ״̬ջ
	std::vector<Symbol*> symbolStack;	// ����ջ

public:
	Parser(const char*);

	void createTable();
	void analysis(const char*);
};

#endif
