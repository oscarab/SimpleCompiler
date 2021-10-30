#ifndef MACHINE_H
#define MACHINE_H

#include "Token.h"
#include "Grammer.h"
#include <vector>
#include <set>
#include <unordered_map>

class Item;
class Symbol;
typedef std::set<Item> State;
typedef std::vector<std::unordered_map<Symbol, int>> Transfer;

class Production {
public:
	int symbolIndex;		// 非终结符
	int productionIndex;	// 产生式

	Production(int, int);
};

// 项目
class Item {
private:
	Production production;	// 产生式
	int point;				// 点 所在位置
	Symbol forward;			// 前瞻
public:
	Item(Production, int, Token);

	Production getProduction();
	int getPoint();
	Symbol getForward();
	void movePoint();

	bool operator<(const Item& item) const;
};

class Machine {
private:
	std::vector<State> states;		// 自动机的所有状态
	Transfer transfer;				// 自动机转移
	Grammer* grammer;				// 文法
public:
	Machine(const char*);

	void solveClosure(int);
	void create();

	~Machine();
};

#endif

