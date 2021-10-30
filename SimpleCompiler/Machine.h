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
	int symbolIndex;		// ���ս��
	int productionIndex;	// ����ʽ

	Production(int, int);
};

// ��Ŀ
class Item {
private:
	Production production;	// ����ʽ
	int point;				// �� ����λ��
	Symbol forward;			// ǰհ
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
	std::vector<State> states;		// �Զ���������״̬
	Transfer transfer;				// �Զ���ת��
	Grammer* grammer;				// �ķ�
public:
	Machine(const char*);

	void solveClosure(int);
	void create();

	~Machine();
};

#endif

