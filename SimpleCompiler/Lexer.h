#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include "Token.h"

namespace Lexical {

	const bool FRONT = 0;
	const bool AFTER = 1;

	// 缓冲区大小为1MB
	const int BLOCK_SIZE = 1 << 20;

	class Lexer;
	class Reader;
	class Scanner;

	// 词法分析器
	class Lexer {
	private:
		
		Reader* codeReader;		// 读取与预处理器
		Scanner* scanner;		// 扫描器

		std::vector<Token> tokens;			// 词法集合

	public:
		Lexer(const char*);

		bool run();							// 词法分析启动
		std::vector<Token>* getTokens();	// 获取词法集合
		void writeTokens(std::ostream&);	// 输出分析出的单词

		~Lexer();
	};

	// 读入缓冲器
	class Reader {
	private:
		
		char* buffer;		// 缓冲区
		std::ifstream fin;	// 文件读入流
		int readin;			// 当前读入字节数
		bool scanPart;		// 当前被扫描的位置

	public:
		Reader(const char*);	// 构造函数

		bool read();			// 数据读取
		void pretreat();		// 预处理
		int getReadin();		// 获取读入字符数量
		bool getScanPart();		// 当前需扫描位置
		char* getBuffer();		// 获取缓冲区

		~Reader();				// 析构函数
	};

	// 扫描器
	class Scanner {
	private:
		
		char* buffer;		// 缓冲区
		int startPoint;		// 起始指针
		int scanPoint;		// 扫描指针
		int endPoint;		// 结束位置
		int isComplete;		// 完整度，为1表示完整，不会返回INCOMPLETE

		std::unordered_map<std::string, int> tablePoint;

		bool isDigit();
		bool isLetter();
		int isReachComplete();
		
		int step();					// 指针前进
		void retract();				// 指针后退
		void align();				// 指针对齐
		void rollback();			// 撤销已经读取的字符
		void skipBlank();			// 跳过空白

		TokenAttribute reserve();	// 查找关键字表
		int insertID();				// 加入新标识符
		int insertConstant();		// 加入新常数
	public:
		Scanner();

		void setBuffer(char*);		// 设置buffer数组
		void setEndPoint(int);		// 设置结束位置
		void setIsComplete(int);	// 设置获取完整度

		Token scan();				// 扫描
	};
}

#endif
