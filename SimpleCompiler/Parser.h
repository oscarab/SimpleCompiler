#ifndef PARSER_H
#define PARSER_H

#include "Machine.h"

struct Action;
class SyntaxNode;
typedef std::vector<std::unordered_map<Symbol, Action>> ParserTable;
typedef std::vector<SyntaxNode*> NodeList;

// 语法树结点
class SyntaxNode {
private:
	Symbol* symbol;
	NodeList child;
public:
	SyntaxNode(Symbol*);
	void addChild(SyntaxNode*);
	NodeList* getChildren();
};

// 语法树
class SyntaxTree {
private:

	SyntaxNode* head;
	NodeList constructStack;

public:
	void insert(Symbol*);
	void construct(Symbol*, int);

	~SyntaxTree();
};

// 动作
struct Action {
	bool action;		// 是否是ACTION
	bool reduction;		// 是否是归约
	bool accept;		// 接受状态
	int go;				// 转移状态
	Production prod;	// 归约用到的产生式

	Action() : prod(NULL, 0, 0) { 
		action = false;
		reduction = false;
		accept = false;
		go = 0;
	}
};

class Parser {
private:

	Machine machine;		// 自动机
	ParserTable table;		// 分析表
	SyntaxTree tree;		// 语法树

	std::vector<int> stateStack;		// 状态栈
	std::vector<Symbol*> symbolStack;	// 符号栈

public:
	Parser(const char*);

	void createTable();				// 建立分析表
	void analysis(const char*);		// 开始语法分析
};

#endif
