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
	bool end;			// �Ƿ�Ϊ�ս��
	Token token;		// ��Ϊ�ս����ʱ��Ч
	String name;		// ��Ϊ���ս��ʱ�ı�Ԫ����

	// ����ʽ�Ҳ�
	std::vector<PSymbol> production;
public:
	Symbol(Token);						// �����ս��
	Symbol(String);						// ������ս��

	bool isEnd() const;					// �Ƿ����ս��
	String getName() const;				// ��ȡ���ս������
	Token getToken() const;				// ��ȡ�ս��Token
	std::vector<PSymbol>* getProductions();
	void insertProduction(PSymbol&);	// �����²���ʽ

	void write(std::ostream&, int);
	bool operator==(const Symbol& symbol) const;
	bool operator<(const Symbol& symbol) const;
};

// Symbol�Ĺ�ϣ����ģ�嶨��
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

	PSymbol symbols;				// ���з��ţ������ս������ս����
	StrMapInt strMapTable;			// �ַ������±����ӳ��ı�
	SymMapInt symMapTable;			// �������±����ӳ��ı�
	PSymbol leftSymbols;			// �����ڲ���ʽ�󲿵ķ���

	SymMapBool canEmpty;			// �ɿյķ��ս��
	SymMapFirst firstSet;			// ÿ�����ս����Ӧ��First����

	std::vector<int> prodCounter;	// ����ʽ���

public:
	Grammer(const char*);					// ��ȡ�ķ��ļ�
	Symbol* insertSymbol(String&, PSymbol*);// �½�Symbol

	void solveCanEmpty();					// ����ɿյķ��ս��
	void solveFirst();						// �������з��ս����First����
	void solveFirst(PSymbol&);				// ������Ŵ���First����

	void getProduction(int, int, PSymbol&);	// ��ȡָ������ʽ
	int getSymbolIndex(Symbol*);			// ���ݷ��Ż�ȡ�±�
	Symbol* getSymbol(String);				// ��ȡ�ַ�����Ӧ�ķ���
	PSymbol* getSymbols();					// �������з���
	int getProductionCount(int);			// ��ȡ����ʽ����

	~Grammer();
};

// �Զ���set�ıȽϺ���
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

