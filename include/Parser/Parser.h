#ifndef PARSER_H
#define PARSER_H

#include "Machine.h"
#include "Lexer/Lexer.h"
#include "Semantic/SemanticAnalyzer.h"
#include "Optimization/Optimization.h"

struct Action;
class SyntaxNode;
typedef std::vector<std::unordered_map<Symbol*, Action>> ParserTable;
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
	Symbol* getSymbol();
	void write(std::ostream&, int, char);
};

// 语法树
class SyntaxTree {
private:

	SyntaxNode* head;
	NodeList constructStack;

public:
	void insert(Symbol*);
	void construct(Symbol*, int);
	SyntaxNode* getHead();

	~SyntaxTree();
};

// 动作
struct Action {
	bool action;		// 是否是ACTION
	bool reduction;		// 是否是归约
	bool accept;		// 接受状态
	int go;				// 转移到的状态
	Production product;	// 归约用到的产生式

	Action() : product(NULL, 0) { 
		action = false;
		reduction = false;
		accept = false;
		go = 0;
	}

	String toString() {
		if (accept) {
			return String("acc");
		}
		if (!action) {
			return std::to_string(go);
		}
		String str = reduction? "r" : "s";
		return str + std::to_string(go);
	}
};

// 分析器
class Parser {
private:

	Machine machine;					// 自动机
	ParserTable table;					// 分析表
	SyntaxTree tree;					// 语法树
	SemanticAnalyzer analyzer;			// 语义分析器
	Optimization optimization;			// 优化器

	std::vector<int> stateStack;		// 状态栈
	std::vector<Symbol*> symbolStack;	// 符号栈

public:
	Parser(const char*);

	void createTable();								// 建立分析表
	bool analysis(Lexical::Lexer*);					// 开始语法分析
	bool optimize();								// 优化

	SyntaxTree* getTree();							// 获取语法树
	void writeTable();								// 输出分析表
	void writeStack(std::ostream&);					// 输出当前栈的内容
};

#endif
