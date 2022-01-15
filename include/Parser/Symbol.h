#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include <vector>
#include "Reflect/ReflectBase.h"
#include "Lexer/Token.h"

class Symbol;
class Token;
typedef std::string String;
typedef std::vector<Symbol*> SymbolChain;

// 符号
class Symbol {
public:
	virtual bool isEnd() const = 0;					// 是否是终结符

	virtual void write(std::ostream&, int) = 0;
	bool operator==(const Symbol& symbol) const;
};

// 终结符
class Terminator : public Symbol, public ReflectBase {
protected:
	Token token;							// 终结符号
public:
	Terminator(Token);						// 构造终结符
	Token getToken() const;					// 获取终结符Token
	bool isEnd() const;

	virtual void write(std::ostream&, int);
	virtual void setupReflect();
};

// 非终结符
class NonTerminator : public Symbol, public ReflectBase {
protected:
	String name;							// 变元名字
	std::vector<SymbolChain> production;	// 该变元作为左侧的产生式
public:
	NonTerminator(String);					// 构造非终结符
	String getName() const;					// 获取非终结符名字
	bool isEnd() const;
	std::vector<SymbolChain>* getProductions();
	void insertProduction(SymbolChain&);	// 插入新产生式

	virtual void write(std::ostream&, int);
	virtual void setupReflect();
};

// Symbol的哈希函数模板定制
namespace std {
	template<>
	class equal_to<Symbol*>{
		public:
           bool operator()(const Symbol* sym1, const Symbol* sym2) const {
			   return *sym1 == *sym2;
           }
	};

	template <>
	class hash<Symbol*> {
	public:
		size_t hasInt(int number) const {
			return (number * 2654435769) >> 28;
		}
		size_t operator()(const Symbol* symbol) const {
			if (symbol->isEnd()) {
				Terminator* terminator = static_cast<Terminator*>(const_cast<Symbol*>(symbol));
				return hasInt((int)terminator->getToken().getType()) ^
					hasInt((int)terminator->getToken().getAttribute());
			}
			else {
				NonTerminator* non_terminator = static_cast<NonTerminator*>(const_cast<Symbol*>(symbol));
				return hash<string>()(non_terminator->getName());
			}
		}
	};
};

#endif
