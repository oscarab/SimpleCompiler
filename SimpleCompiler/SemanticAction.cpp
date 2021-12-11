#include "SemanticAction.h"
#include "SemanticAnalyzer.h"
#include "Symbol.h"
#include <unordered_map>

extern std::unordered_map<std::string, void (*)(SemanticAnalyzer*, std::vector<Property>&)> functionsPoint;

void SemanticAction::setOperator(SemanticOperator op) {
	this->op = op;
}

void SemanticAction::setFunctionExecutor(std::string func) {
	if (functionsPoint[func] == NULL) {
		functionExecutor = NULL;
	}
	functionExecutor = functionsPoint[func];
}

void SemanticAction::addProperties(Property property) {
	properties.push_back(property);
}

/**
 * @brief 执行语义动作
 * @param analyzer 语义分析器
*/
void SemanticAction::execute(SemanticAnalyzer* analyzer) {
	if (op == SemanticOperator::ASSIGN) {
		Symbol* symbol1 = analyzer->getSymbolFromStack(properties[0].index);
		NonTerminator* left = static_cast<NonTerminator*>(symbol1);

		if (properties[1].index > 0) {
			// 代表右值为 属性
			Symbol* symbol2 = analyzer->getSymbolFromStack(properties[1].index);
			if (symbol2->isEnd()) {
				Terminator* right = static_cast<Terminator*>(symbol2);
				left->setFields(properties[0].property, right->getFields(properties[1].property));
			}
			else {
				NonTerminator* right = static_cast<NonTerminator*>(symbol2);
				left->setFields(properties[0].property, right->getFields(properties[1].property));
			}
		}
		else if (properties[1].index < 0) {
			// 代表右值为 数字
			int num = std::stoi(properties[1].property);
			left->setFields(properties[0].property, (char*)&num);
		}
		else {
			// 代表右值为 字符串
			left->setFields(properties[0].property, (char*) &properties[1].property);
		}
	}
	else if (op == SemanticOperator::FUNCTION) {
		functionExecutor(analyzer, properties);
	}
	else {
		std::vector<Property> args = std::vector<Property>(properties.begin() + 1, properties.end());
		functionExecutor(analyzer, args);

		Symbol* symbol = analyzer->getSymbolFromStack(properties[0].index);
		NonTerminator* left = static_cast<NonTerminator*>(symbol);

		if (args.back().index == 1) {
			// 返回值是一个数字
			args.pop_back();
			int result = std::stoi(args.back().property);
			left->setFields(properties[0].property, (char*)&result);
		}
		else {
			// 返回值是字符串
			args.pop_back();
			left->setFields(properties[0].property, (char*)&args.back().property);
		}
		
	}
}