#ifndef GRAMMER_H
#define GRAMMER_H

#include "Token.h"
#include "Symbol.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <set>

struct compare;
typedef std::unordered_map<String, int> StrMapInt;
typedef std::unordered_map<Symbol*, int> SymMapInt;
typedef std::unordered_map<Symbol*, std::set<Symbol*, compare>> SymMapFirst;
typedef std::unordered_map<Symbol*, bool> SymMapBool;

class Grammer {
private:

	SymbolChain symbols;				// ���з��ţ������ս������ս����
	StrMapInt strMapTable;				// �ַ������±����ӳ��ı�
	SymMapInt symMapTable;				// �������±����ӳ��ı�
	SymbolChain leftSymbols;			// �����ڲ���ʽ�󲿵ķ���

	SymMapBool canEmpty;			// �ɿյķ��ս��
	SymMapFirst firstSet;			// ÿ�����ս����Ӧ��First����

	std::vector<int> prodCounter;	// ����ʽ���

public:
	Grammer(const char*);						// ��ȡ�ķ��ļ�
	Symbol* insertSymbol(String&, SymbolChain*);// �½�Symbol

	void solveCanEmpty();						// ����ɿյķ��ս��
	void solveFirst();							// �������з��ս����First����
	void solveFirst(SymbolChain&);				// ������Ŵ���First����

	Symbol* getSymbol(String);					// ��ȡ�ַ�����Ӧ�ķ���
	SymbolChain* getSymbols();					// �������з���
	int getProductionCount(Symbol*);			// ��ȡ����ʽ����

	~Grammer();
};

// �Զ���set�ıȽϺ���
struct compare {
	bool operator() (const Symbol* a, const Symbol* b) const {
		return a < b;
	}
};

#endif

