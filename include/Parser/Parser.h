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

// �﷨�����
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

// �﷨��
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

// ����
struct Action {
	bool action;		// �Ƿ���ACTION
	bool reduction;		// �Ƿ��ǹ�Լ
	bool accept;		// ����״̬
	int go;				// ת�Ƶ���״̬
	Production product;	// ��Լ�õ��Ĳ���ʽ

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

// ������
class Parser {
private:

	Machine machine;					// �Զ���
	ParserTable table;					// ������
	SyntaxTree tree;					// �﷨��
	SemanticAnalyzer analyzer;			// ���������
	Optimization optimization;			// �Ż���

	std::vector<int> stateStack;		// ״̬ջ
	std::vector<Symbol*> symbolStack;	// ����ջ

public:
	Parser(const char*);

	void createTable();								// ����������
	bool analysis(Lexical::Lexer*);					// ��ʼ�﷨����
	bool optimize();								// �Ż�

	SyntaxTree* getTree();							// ��ȡ�﷨��
	void writeTable();								// ���������
	void writeStack(std::ostream&);					// �����ǰջ������
};

#endif
