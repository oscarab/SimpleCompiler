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

// ����ʽ
class Production {
public:
	Symbol* symbolPoint;	// ���ս��
	int symbolIndex;
	int productionIndex;	// ����ʽ

	Production(Symbol*, int, int);
};

// ��Ŀ
class Item {
private:

	Production production;	// ����ʽ
	int point;				// �� ����λ��
	Symbol* forward;		// ǰհ���϶����ս����

public:
	Item(Production, int, Symbol*);

	Production getProduction() const;
	int getPoint() const;
	Symbol* getForward();
	void movePoint();				// �� ����
	bool isReduction() const;		// ����������ַ����Ƿ���Թ�Լ
	Symbol* isMoveIn() const;		// ����������ַ����Ƿ��������
	bool isAccept() const;			// ����������ַ����Ƿ�������ս���

	bool operator<(const Item&) const;
	bool operator==(const Item&) const;
};

// �Զ���
class Machine {
private:

	std::vector<State> states;		// �Զ���������״̬
	Transfer transfer;				// �Զ���ת��
	Grammer* grammer;				// �ķ�

public:
	Machine(const char*);

	void solveClosure(State&);		// ����closure��
	void create();					// �����Զ���

	std::vector<State>* getStates();// ��ȡ����״̬
	Transfer* getTransfer();		// ��ȡת��
	Grammer* getGrammer();			// ��ȡ�ķ�

	~Machine();
};

#endif

