#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>

// 单词种类
enum class TokenType {
	KEY_WORD,	// 关键字
	ID,			// 标识符
	OPERATOR,	// 运算符
	CONSTANT,	// 常数
	BOUNDARY,	// 界符
	BRACKET,	// 括号
	END,		// 终结#
	EPSILON,	// 空

	FAIL,		// 分析失败
	INCOMPLETE,	// 未完成
	COMPLETE	// 完成
};

// 单词属性
enum class TokenAttribute {
	_int, _float, _void, _if, _else, _while, _return,
	Add, Minus, Multiply, Divide, Assign, Equal, Greater, Less, Gequal, Lequal, Nequal,
	AND, OR,
	Comma, Semicolon,
	LeftBrace, RightBrace, LeftBracket, RightBracket, LeftSquare, RightSquare,
	RealConstant, RealID, None
};

class Token {
private:
	TokenType type;
	TokenAttribute attribute;

	int index;		// 表指针
	int row, col;	// 在源码中的行、列位置
public:
	// 构造函数
	Token(TokenType, TokenAttribute, int = -1, int = -1);
	Token(TokenType, TokenAttribute, int, int, int);

	int getIndex();						// 获取表内位置
	TokenType getType();				// 获取单词种类
	TokenAttribute getAttribute();		// 获取单词属性
	bool isEmpty();						// 是否为空

	void write(std::ostream&, int, bool);
	void setIndex(int);

	bool operator==(const Token& token) const;
};

#endif