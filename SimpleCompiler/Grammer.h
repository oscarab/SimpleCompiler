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
typedef std::unordered_map<Symbol, std::set<Symbol*>> SymMapFirst;
typedef std::unordered_map<Symbol, bool> SymMapBool;

namespace std {
	template <>
	class hash<Symbol> {
	public:
		size_t operator()(Symbol& symbol) const {
			return hash<bool>()(symbol.isEnd()) ^ 
				hash<string>()(symbol.getName()) ^
				hash<int>()((int) symbol.getToken().getType()) ^
				hash<int>()((int)symbol.getToken().getAttribute()) ^
				hash<int>()(symbol.getToken().getIndex());
		}
	};
};

class Symbol {
private:
	bool end;			// �Ƿ�Ϊ�ս��
	Token token;		// ��Ϊ�ս����ʱ��Ч
	String name;		// ��Ϊ���ս��ʱ�ı�Ԫ����

	// ����ʽ�Ҳ�
	std::vector<PSymbol> production;
public:
	Symbol(Token);
	Symbol(String);

	bool isEnd();
	String getName();
	Token& getToken();
	void insertProduction(PSymbol&);	// �����²���ʽ

	bool operator==(const Symbol& symbol) const;
};

class Grammer {
private:
	PSymbol symbols;			// ���з��ţ������ս������ս����
	StrMapInt mappingTable;		// �ַ�������Ž���ӳ��ı�
	PSymbol rightSymbols;		// �����ڲ���ʽ�󲿵ķ���

	SymMapBool canEmpty;		// �ɿյķ��ս��
	SymMapFirst firstSet;		// ÿ�����ս����Ӧ��First����
public:
	Grammer(const char*);

	void solveCanEmpty();		// ����ɿյķ��ս��
	void solveFirst();			// �������з��ս����First����
};

#endif

