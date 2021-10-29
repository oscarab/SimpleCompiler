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
	int symbolPoint;		// ���ս��
	int productionPoint;	// ����ʽ
public:
	Production(int, int);
};

// ��Ŀ
class Item {
private:
	Production production;	// ����ʽ
	int point;				// �� ����λ��
	Token forward;			// ǰհ
};

class Machine {
private:
	std::vector<State> states;		// �Զ���������״̬
	Transfer transfer;				// �Զ���ת��
	Grammer grammer;				// �ķ�
public:
	Machine();

	void solveClosure();
	void create();
};

#endif

