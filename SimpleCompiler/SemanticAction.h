#ifndef SEMANTICACTION_H
#define SEMANTICACTION_H

#include <vector>
#include <string>

class SemanticAnalyzer;

enum class SemanticOperator {
	FUNCTION, ASSIGN, ALL
};

struct Property {
	int index;
	std::string property;
};

// ���嶯��
class SemanticAction {
private:
	SemanticOperator op;
	void (*functionExecutor)(SemanticAnalyzer*, std::vector<Property>&);
	std::vector<Property> properties;
	/*
	* �ڷŹ���
	* ASSIGN����ֵ ��ֵ
	* FUNCTION������1 ����2 ...
	* ALL����ֵ ����1 ����2 ...
	* ����ֵ�ᱻѹ����ĩβ
	*/
public:
	void setOperator(SemanticOperator);
	void setFunctionExecutor(std::string);
	void addProperties(Property);
	void execute(SemanticAnalyzer*);
};

#endif // !SEMANTICACTION_H

