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
	bool end;			// �Ƿ�Ϊ�ս��
	Token token;		// ��Ϊ�ս����ʱ��Ч
	String name;		// ��Ϊ���ս��ʱ�ı�Ԫ����

	// ����ʽ�Ҳ�
	std::vector<PSymbol> production;
public:
	Symbol(Token);	//
	Symbol(String);	//

	bool isEnd() const;			//
	String getName() const;		//
	Token getToken() const;		//
	std::vector<PSymbol>* getProductions();
	void insertProduction(PSymbol&);	// �����²���ʽ

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
	PSymbol symbols;			// ���з��ţ������ս������ս����
	StrMapInt strMapTable;		// �ַ������±����ӳ��ı�
	SymMapInt symMapTable;		// �������±����ӳ��ı�
	PSymbol leftSymbols;		// �����ڲ���ʽ�󲿵ķ���

	SymMapBool canEmpty;		// �ɿյķ��ս��
	SymMapFirst firstSet;		// ÿ�����ս����Ӧ��First����
public:
	Grammer(const char*);		// const char*�ļ��� ���ļ�

	String removeBrackets(String);	// �Ƴ����ս���ļ�����
	Token setToken(String);		// �����ս���ַ���ΪToken��ʽ

	void solveCanEmpty();		// ����ɿյķ��ս��
	void solveFirst();			// �������з��ս����First����
	void solveFirst(PSymbol&);	// ���ԭ���� ��Ϊfirst

	void getProduction(int, int, PSymbol&);	// �ڼ������ս�� �ڼ�������ʽ
	int getSymbolIndex(Symbol*);
	PSymbol* getSymbols();		// ����symbolsָ��

	~Grammer();
};

#endif

