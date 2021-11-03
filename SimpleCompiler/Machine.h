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
typedef std::vector<SymMapInt> Transfer;

class Production {
public:
	Symbol* symbolPoint;	// 非终结符
	int symbolIndex;
	int productionIndex;	// 产生式

	Production(Symbol*, int, int);
};

// 项目
class Item {
private:

	Production production;	// 产生式
	int point;				// 点 所在位置
	Symbol forward;			// 前瞻（肯定是终结符）

public:
	Item(Production, int, Token);

	Production getProduction() const;
	int getPoint() const;
	Symbol* getForward();
	void movePoint();				// 点 后移
	bool isReduction() const;		// 对于输入的字符，是否可以归约
	Symbol* isMoveIn() const;		// 对于输入的字符，是否可以移入
	bool isAccept() const;			// 对于输入的字符，是否可以最终接受

	bool operator<(const Item&) const;
	bool operator==(const Item&) const;
};

class Machine {
private:

	std::vector<State> states;		// 自动机的所有状态
	Transfer transfer;				// 自动机转移
	Grammer* grammer;				// 文法

public:
	Machine(const char*);

	void solveClosure(State&);		// 计算closure集
	void create();					// 构建自动机

	std::vector<State>* getStates();// 获取所有状态
	Transfer* getTransfer();		// 获取转移
	Grammer* getGrammer();			// 获取文法

	~Machine();
};

#endif

