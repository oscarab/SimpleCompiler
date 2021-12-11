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

// 语义动作
class SemanticAction {
private:
	SemanticOperator op;
	void (*functionExecutor)(SemanticAnalyzer*, std::vector<Property>&);
	std::vector<Property> properties;
	/*
	* 摆放规则
	* ASSIGN：左值 右值
	* FUNCTION：参数1 参数2 ...
	* ALL：左值 参数1 参数2 ...
	* 返回值会被压在最末尾
	*/
public:
	void setOperator(SemanticOperator);
	void setFunctionExecutor(std::string);
	void addProperties(Property);
	void execute(SemanticAnalyzer*);
};

#endif // !SEMANTICACTION_H

