#ifndef MACHINE_H
#define MACHINE_H

#include "Token.h"
#include "Grammer.h"
#include <vector>
#include <unordered_map>

typedef std::vector<Item> State;
typedef std::vector<std::unordered_map<Symbol, int>> Transfer;

class Production {
private:
	int symbolPoint;		// 非终结符
	int productionPoint;	// 产生式
public:
	Production(int, int);
};

// 项目
class Item {
private:
	Production production;	// 产生式
	int point;				// 点 所在位置
	Token forward;			// 前瞻
};

class Machine {
private:
	std::vector<State> states;		// 自动机的所有状态
	Transfer transfer;				// 自动机转移
	Grammer grammer;				// 文法
public:
	Machine();

	void solveClosure();
	void create();
};

#endif

