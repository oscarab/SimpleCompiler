#ifndef TOKEN_H
#define TOKEN_H

// 单词种类
enum class TokenType {
	KEY_WORD,	// 关键字
	ID,			// 标识符
	OPERATOR,	// 运算符
	CONSTANT,	// 常数
	BOUNDARY,	// 界符
	BRACKET,	// 括号
	END,		// 终结#

	FAIL,		// 分析失败
	INCOMPLETE,	// 未完成
	COMPLETE	// 完成
};

// 单词属性
enum class TokenAttribute {
	_int, _void, _if, _else, _while, _return,
	Add, Minus, Multiply, Divide, Assign, Equal, Greater, Less, Gequal, Lequal, Nequal,
	Comma, Semicolon,
	LeftBrace, RightBrace, LeftBracket, RightBracket,
	RealConstant, RealID, None
};

class Token {
private:
	TokenType type;
	TokenAttribute attribute;

	// 表指针
	int index;
public:
	// 构造函数
	Token(TokenType, TokenAttribute);
	Token(TokenType, TokenAttribute, int);

	int getIndex();
	TokenType getType();
	TokenAttribute getAttribute();

	bool operator==(const Token& token) const;
};

#endif