#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <string>
#include <fstream>

namespace LexicalAnalysis {

	const bool FRONT = 0;
	const bool AFTER = 1;

	// ��������СΪ1MB
	const int BLOCK_SIZE = 1 << 20;

	// ��������
	enum class TokenType {
		KEY_WORD,	// �ؼ���
		ID,			// ��ʶ��
		OPERATOR,	// �����
		CONSTANT,	// ����
		BOUNDARY,	// ���
		BRACKET,	// ����

		FAIL,		// ����ʧ��
		INCOMPLETE,	// δ���
		COMPLETE	// ���
	};

	// ��������
	enum class TokenAttribute {
		_int, _void, _if, _else, _while, _return,
		RealString,
		Add, Minus, Multiply, Divide, Assign, Equal, Greater, Less, Gequal, Lequal, Nequal,
		RealConstant,
		Comma, Semicolon,
		LeftBrace, RightBrace, LeftBracket, RightBracket,
		_others, _fail, _incomplete, _complete
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

		int getIndex();
		TokenType getType();
		TokenAttribute getAttribute();

		friend std::ostream& operator<<(std::ostream&, const Token&);
	};

	// �ʷ�������
	class Lexer {
	private:
		// ��ȡ��Ԥ������
		Reader* codeReader;

		// ɨ����
		Scanner* scanner;

		// ��ʶ����
		std::vector<std::string> idTable;

		// ������
		std::vector<double> constantTable;

		// �ʷ�����
		std::vector<Token> tokens;
	public:
		Lexer(const char*);

		// �ʷ���������
		void run();

		// ��ȡ�ʷ�����
		std::vector<Token> getTokens();

		~Lexer();
	};

	// ���뻺����
	class Reader {
	private:
		// ������
		char* buffer;

		// �ļ�������
		std::ifstream fin;

		// ��ǰ�����ֽ���
		int readin;

		// ��ǰ��ɨ���λ��
		bool scanPart;

	public:
		// ���캯��
		Reader(const char*);

		// ���ݶ�ȡ
		bool read();

		// Ԥ����
		void pretreat();

		// ��ȡ�����ַ�����
		int getReadin();

		// ��ǰ��ɨ��λ��
		bool getScanPart();

		// ��������
		~Reader();
	};

	// ɨ����
	class Scanner {
	private:
		// ������
		char* buffer;

		// ��ʼָ��
		int startPoint;

		// ɨ��ָ��
		int scanPoint;

		// ����λ��
		int endPoint;

		// �����ȣ�Ϊ1��ʾ���������᷵��INCOMPLETE
		int isComplete;

		// ��ʶ�����볣���������
		std::vector<std::string>* idTable;
		std::vector<double>* constantTable;

		bool isDigit();
		bool isLetter();
	public:
		Scanner(std::vector<std::string>*, std::vector<double>*);

		// ��ȡbuffer����
		void setBuffer(char*);

		// ��ȡ����λ��
		void setEndPoint(int);

		// ��ȡ������
		void setIsComplete(int);

		// ָ��ǰ��
		void step();

		// ָ�����
		void retract();

		// �����հ�
		void skipBlank();

		// ���ҹؼ��ֱ�
		TokenAttribute reserve();

		// �����±�ʶ��
		int insertID();

		// �����³���
		int insertConstant();

		// ��װToken
		//Token createToken();

		// ɨ��
		Token scan();
	};
}

#endif
