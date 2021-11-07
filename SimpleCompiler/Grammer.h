#ifndef GRAMMER_H
#define GRAMMER_H

#include "Token.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <set>

class Symbol;
struct compare;
typedef std::string String;
typedef std::vector<Symbol*> PSymbol;
typedef std::unordered_map<String, int> StrMapInt;
typedef std::unordered_map<Symbol, int> SymMapInt;
typedef std::unordered_map<Symbol, std::set<Symbol*, compare>> SymMapFirst;
typedef std::unordered_map<Symbol, bool> SymMapBool;

class Symbol {
private:
	bool end;			// 是否为终结符
	Token token;		// 当为终结符号时有效
	String name;		// 当为非终结符时的变元名字

	// 产生式右部
	std::vector<PSymbol> production;
public:
	Symbol(Token);						// 构造终结符
	Symbol(String);						// 构造非终结符

	bool isEnd() const;					// 是否是终结符
	String getName() const;				// 获取非终结符名字
	Token getToken() const;				// 获取终结符Token
	std::vector<PSymbol>* getProductions();
	void insertProduction(PSymbol&);	// 插入新产生式

	void write(std::ostream&, int);
	bool operator==(const Symbol& symbol) const;
	bool operator<(const Symbol& symbol) const;
};

// Symbol的哈希函数模板定制
namespace std {
	template <>
	class hash<Symbol> {
	public:
		size_t operator()(const Symbol& symbol) const {
			return hash<bool>()(symbol.isEnd()) ^
				hash<string>()(symbol.getName()) ^
				hash<int>()((int)symbol.getToken().getType()) ^
				hash<int>()((int)symbol.getToken().getAttribute()) ^
				hash<int>()(symbol.getToken().getIndex());
		}
	};
};

class Grammer {
private:

	PSymbol symbols;				// 所有符号（包括终结符与非终结符）
	StrMapInt strMapTable;			// 字符串与下标进行映射的表
	SymMapInt symMapTable;			// 符号与下标进行映射的表
	PSymbol leftSymbols;			// 出现在产生式左部的符号

	SymMapBool canEmpty;			// 可空的非终结符
	SymMapFirst firstSet;			// 每个非终结符对应的First集合

	std::vector<int> prodCounter;	// 产生式序号

public:
	Grammer(const char*);					// 读取文法文件
	Symbol* insertSymbol(String&, PSymbol*);// 新建Symbol

	void solveCanEmpty();					// 计算可空的非终结符
	void solveFirst();						// 计算所有非终结符的First集合
	void solveFirst(PSymbol&);				// 计算符号串的First集合

	void getProduction(int, int, PSymbol&);	// 获取指定产生式
	int getSymbolIndex(Symbol*);			// 根据符号获取下标
	Symbol* getSymbol(String);				// 获取字符串对应的符号
	PSymbol* getSymbols();					// 返回所有符号
	int getProductionCount(int);			// 获取产生式计数

	~Grammer();
};

// 自定义set的比较函数
struct compare {
	bool operator() (const Symbol* a, const Symbol* b) const {
		if (a->isEnd() != b->isEnd()) {
			return a->isEnd() < b->isEnd();
		}

		if (a->getToken() == b->getToken()) {
			return a->getName() < b->getName();
		}
		else {
			return a->getToken() < b->getToken();
		}
	}
};

#endif

