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

	SymbolChain symbols;				// 所有符号（包括终结符与非终结符）
	StrMapInt strMapTable;				// 字符串与下标进行映射的表
	SymMapInt symMapTable;				// 符号与下标进行映射的表
	SymbolChain leftSymbols;			// 出现在产生式左部的符号

	SymMapBool canEmpty;			// 可空的非终结符
	SymMapFirst firstSet;			// 每个非终结符对应的First集合

	std::vector<int> prodCounter;	// 产生式序号

public:
	Grammer(const char*);						// 读取文法文件
	Symbol* insertSymbol(String&, SymbolChain*);// 新建Symbol

	void solveCanEmpty();						// 计算可空的非终结符
	void solveFirst();							// 计算所有非终结符的First集合
	void solveFirst(SymbolChain&);				// 计算符号串的First集合

	Symbol* getSymbol(String);					// 获取字符串对应的符号
	SymbolChain* getSymbols();					// 返回所有符号
	int getProductionCount(Symbol*);			// 获取产生式计数

	~Grammer();
};

// 自定义set的比较函数
struct compare {
	bool operator() (const Symbol* a, const Symbol* b) const {
		return a < b;
	}
};

#endif

