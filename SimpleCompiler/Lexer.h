#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <string>
#include <fstream>

namespace LexicalAnalysis {

	const bool FRONT = 0;
	const bool AFTER = 1;

	// 缓冲区大小为1MB
	const int BLOCK_SIZE = 1 << 20;

	// 单词种类
	enum class TokenType {
		KEY_WORD,	// 关键字
		ID,			// 标识符
		OPERATOR,	// 运算符
		CONSTANT,	// 常数
		BOUNDARY,	// 界符
		BRACKET,	// 括号

		FAIL,		// 分析失败
		INCOMPLETE,	// 未完成
		COMPLETE	// 完成
	};

	// 单词属性
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

		// 表指针
		int index;
	public:
		// 构造函数
		Token(TokenType, TokenAttribute);
		Token(TokenType, TokenAttribute, int);

		int getIndex();
		TokenType getType();
		TokenAttribute getAttribute();

		friend std::ostream& operator<<(std::ostream&, const Token&);
	};

	// 词法分析器
	class Lexer {
	private:
		// 读取与预处理器
		Reader* codeReader;

		// 扫描器
		Scanner* scanner;

		// 标识符表
		std::vector<std::string> idTable;

		// 常数表
		std::vector<double> constantTable;

		// 词法集合
		std::vector<Token> tokens;
	public:
		Lexer(const char*);

		// 词法分析启动
		void run();

		// 获取词法集合
		std::vector<Token> getTokens();

		~Lexer();
	};

	// 读入缓冲器
	class Reader {
	private:
		// 缓冲区
		char* buffer;

		// 文件读入流
		std::ifstream fin;

		// 当前读入字节数
		int readin;

		// 当前被扫描的位置
		bool scanPart;

	public:
		// 构造函数
		Reader(const char*);

		// 数据读取
		bool read();

		// 预处理
		void pretreat();

		// 获取读入字符数量
		int getReadin();

		// 当前需扫描位置
		bool getScanPart();

		// 析构函数
		~Reader();
	};

	// 扫描器
	class Scanner {
	private:
		// 缓冲区
		char* buffer;

		// 起始指针
		int startPoint;

		// 扫描指针
		int scanPoint;

		// 结束位置
		int endPoint;

		// 完整度，为1表示完整，不会返回INCOMPLETE
		int isComplete;

		// 标识符表与常数表的引用
		std::vector<std::string>* idTable;
		std::vector<double>* constantTable;

		bool isDigit();
		bool isLetter();
	public:
		Scanner(std::vector<std::string>*, std::vector<double>*);

		// 获取buffer数组
		void setBuffer(char*);

		// 获取结束位置
		void setEndPoint(int);

		// 获取完整度
		void setIsComplete(int);

		// 指针前进
		void step();

		// 指针后退
		void retract();

		// 跳过空白
		void skipBlank();

		// 查找关键字表
		TokenAttribute reserve();

		// 加入新标识符
		int insertID();

		// 加入新常数
		int insertConstant();

		// 组装Token
		//Token createToken();

		// 扫描
		Token scan();
	};
}

#endif
