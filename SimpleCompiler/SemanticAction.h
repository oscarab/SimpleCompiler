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

class SemanticAction {
private:
	SemanticOperator op;
	void (*functionExecutor)(SemanticAnalyzer*, std::vector<Property>&);
	std::vector<Property> properties;
public:
	SemanticOperator getOperator();
};

#endif // !SEMANTICACTION_H

