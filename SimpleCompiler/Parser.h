#ifndef PARSER_H
#define PARSER_H

#include "Machine.h"

struct Action;
class SyntaxNode;
typedef std::vector<std::unordered_map<Symbol, Action>> ParserTable;
typedef std::vector<SyntaxNode*> NodeList;

// �﷨�����
class SyntaxNode {
private:
	Symbol* symbol;
	NodeList child;
public:
	SyntaxNode(Symbol*);
	void addChild(SyntaxNode*);
	NodeList* getChildren();
};

// �﷨��
class SyntaxTree {
private:

	SyntaxNode* head;
	NodeList constructStack;

public:
	void insert(Symbol*);
	void construct(Symbol*, int);

	~SyntaxTree();
};

// ����
struct Action {
	bool action;		// �Ƿ���ACTION
	bool reduction;		// �Ƿ��ǹ�Լ
	bool accept;		// ����״̬
	int go;				// ת��״̬
	Production prod;	// ��Լ�õ��Ĳ���ʽ

	Action() : prod(NULL, 0, 0) { 
		action = false;
		reduction = false;
		accept = false;
		go = 0;
	}
};

class Parser {
private:

	Machine machine;		// �Զ���
	ParserTable table;		// ������
	SyntaxTree tree;		// �﷨��

	std::vector<int> stateStack;		// ״̬ջ
	std::vector<Symbol*> symbolStack;	// ����ջ

public:
	Parser(const char*);

	void createTable();				// ����������
	void analysis(const char*);		// ��ʼ�﷨����
};

#endif
