#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include <vector>

class Symbol;
class Token;
typedef std::string String;
typedef std::vector<Symbol*> SymbolChain;

// ����
class Symbol {
public:
	virtual bool isEnd() const = 0;					// �Ƿ����ս��

	virtual void write(std::ostream&, int) = 0;
	bool operator==(const Symbol& symbol) const;
};

// �ս��
class Terminator : public Symbol{
protected:
	Token token;							// �ս����
public:
	Terminator(Token);						// �����ս��
	Token getToken() const;					// ��ȡ�ս��Token
	bool isEnd() const;

	void write(std::ostream&, int);
};

// ���ս��
class NonTerminator : public Symbol {
protected:
	String name;							// ��Ԫ����
	std::vector<SymbolChain> production;	// �ñ�Ԫ��Ϊ���Ĳ���ʽ
public:
	NonTerminator(String);					// ������ս��
	String getName() const;					// ��ȡ���ս������
	bool isEnd() const;
	std::vector<SymbolChain>* getProductions();
	void insertProduction(SymbolChain&);	// �����²���ʽ

	void write(std::ostream&, int);
};

// Symbol�Ĺ�ϣ����ģ�嶨��
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
