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

	// ��������СΪ1MB
	const int BLOCK_SIZE = 1 << 20;

	class Lexer;
	class Reader;
	class Scanner;

	// �ʷ�������
	class Lexer {
	private:
		
		Reader* codeReader;		// ��ȡ��Ԥ������
		Scanner* scanner;		// ɨ����

		std::vector<Token> tokens;			// �ʷ�����

	public:
		Lexer(const char*);

		bool run();							// �ʷ���������
		std::vector<Token>* getTokens();	// ��ȡ�ʷ�����
		void writeTokens(std::ostream&);	// ����������ĵ���

		~Lexer();
	};

	// ���뻺����
	class Reader {
	private:
		
		char* buffer;		// ������
		std::ifstream fin;	// �ļ�������
		int readin;			// ��ǰ�����ֽ���
		bool scanPart;		// ��ǰ��ɨ���λ��

	public:
		Reader(const char*);	// ���캯��

		bool read();			// ���ݶ�ȡ
		void pretreat();		// Ԥ����
		int getReadin();		// ��ȡ�����ַ�����
		bool getScanPart();		// ��ǰ��ɨ��λ��
		char* getBuffer();		// ��ȡ������

		~Reader();				// ��������
	};

	// ɨ����
	class Scanner {
	private:
		
		char* buffer;		// ������
		int startPoint;		// ��ʼָ��
		int scanPoint;		// ɨ��ָ��
		int endPoint;		// ����λ��
		int isComplete;		// �����ȣ�Ϊ1��ʾ���������᷵��INCOMPLETE

		std::unordered_map<std::string, int> tablePoint;

		bool isDigit();
		bool isLetter();
		int isReachComplete();
		
		int step();					// ָ��ǰ��
		void retract();				// ָ�����
		void align();				// ָ�����
		void rollback();			// �����Ѿ���ȡ���ַ�
		void skipBlank();			// �����հ�

		TokenAttribute reserve();	// ���ҹؼ��ֱ�
		int insertID();				// �����±�ʶ��
		int insertConstant();		// �����³���
	public:
		Scanner();

		void setBuffer(char*);		// ����buffer����
		void setEndPoint(int);		// ���ý���λ��
		void setIsComplete(int);	// ���û�ȡ������

		Token scan();				// ɨ��
	};
}

#endif
