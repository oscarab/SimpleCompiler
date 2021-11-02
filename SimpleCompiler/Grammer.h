#ifndef GRAMMER_H
#define GRAMMER_H

#include "Token.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <set>

class Symbol;
typedef std::string String;
typedef std::vector<Symbol*> PSymbol;
typedef std::unordered_map<String, int> StrMapInt;
typedef std::unordered_map<Symbol, int> SymMapInt;
typedef std::unordered_map<Symbol, std::set<Symbol*>> SymMapFirst;
typedef std::unordered_map<Symbol, bool> SymMapBool;

class Symbol {
private:
	bool end;			// 是否为终结符
	Token token;		// 当为终结符号时有效
	String name;		// 当为非终结符时的变元名字

	// 产生式右部
	std::vector<PSymbol> production;
public:
	Symbol(Token);	//
	Symbol(String);	//

	bool isEnd() const;			//
	String getName() const;		//
	Token getToken() const;		//
	std::vector<PSymbol>* getProductions();
	void insertProduction(PSymbol&);	// 插入新产生式

	bool operator==(const Symbol& symbol) const;
};

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
	PSymbol symbols;			// 所有符号（包括终结符与非终结符）
	StrMapInt strMapTable;		// 字符串与下标进行映射的表
	SymMapInt symMapTable;		// 符号与下标进行映射的表
	PSymbol leftSymbols;		// 出现在产生式左部的符号

	SymMapBool canEmpty;		// 可空的非终结符
	SymMapFirst firstSet;		// 每个非终结符对应的First集合
public:
	Grammer(const char*);		// const char*文件名 读文件

	String removeBrackets(String);	// 移除非终结符的尖括号
	Token setToken(String);		// 设置终结符字符串为Token格式

	void solveCanEmpty();		// 计算可空的非终结符
	void solveFirst();			// 计算所有非终结符的First集合
	void solveFirst(PSymbol&);	// 清掉原来的 改为first

	void getProduction(int, int, PSymbol&);	// 第几个非终结符 第几个产生式
	int getSymbolIndex(Symbol*);
	PSymbol* getSymbols();		// 返回symbols指针

	~Grammer();
};

#endif

