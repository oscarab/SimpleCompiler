#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>

// ��������
enum class TokenType {
	KEY_WORD,	// �ؼ���
	ID,			// ��ʶ��
	OPERATOR,	// �����
	CONSTANT,	// ����
	BOUNDARY,	// ���
	BRACKET,	// ����
	END,		// �ս�#
	EPSILON,	// ��

	FAIL,		// ����ʧ��
	INCOMPLETE,	// δ���
	COMPLETE	// ���
};

// ��������
enum class TokenAttribute {
	_int, _void, _if, _else, _while, _return,
	Add, Minus, Multiply, Divide, Assign, Equal, Greater, Less, Gequal, Lequal, Nequal,
	AND, OR,
	Comma, Semicolon,
	LeftBrace, RightBrace, LeftBracket, RightBracket,
	RealConstant, RealID, None
};

class Token {
private:
	TokenType type;
	TokenAttribute attribute;

	// ��ָ��
	int index;
public:
	// ���캯��
	Token(TokenType, TokenAttribute);
	Token(TokenType, TokenAttribute, int);

	int getIndex();						// ��ȡ����λ��
	TokenType getType();				// ��ȡ��������
	TokenAttribute getAttribute();		// ��ȡ��������
	bool isEmpty();						// �Ƿ�Ϊ��

	void write(std::ostream&, int, bool);
	void setIndex(int);

	bool operator==(const Token& token) const;
};

#endif