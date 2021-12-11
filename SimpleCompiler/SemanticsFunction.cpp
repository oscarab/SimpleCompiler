#include <vector>
#include <unordered_map>
#include "SemanticAction.h"
#include "SemanticAnalyzer.h"
#include "Symbol.h"

using namespace std;

/**
 * @brief 反包装
 * @param analyzer 
 * @param property 
 * @return 反包装得到字符串
*/
string unwrap(SemanticAnalyzer* analyzer, Property property) {
	if (property.index == 0) {
		return property.property;
	}
	Symbol* symbol = analyzer->getSymbolFromStack(property.index);
	if (symbol->isEnd()) {
		Terminator* terminator = static_cast<Terminator*>(symbol);
		return *((string*) terminator->getFields(property.property));
	}
	NonTerminator* non_terminator = static_cast<NonTerminator*>(symbol);
	if (non_terminator->getFieldType(property.property) == "string") {
		return *((string*)non_terminator->getFields(property.property));
	}
	else {
		return std::to_string(*((int*)non_terminator->getFields(property.property)));
	}
	
}

/**
 * @brief 创建新临时变量
 * @param analyzer 语义分析器
 * @param properties 存放新建的临时变量
*/
void newtemp(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	properties.push_back(Property{ 0, analyzer->newtemp() });
	properties.push_back(Property{ 0, "" });
}

/**
 * @brief 检查符号表，看此符号是否被定义了
 * @param analyzer 语义分析器
 * @param properties 存放该符号的相对地址
*/
void lookup(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	string name = unwrap(analyzer, properties[0]);
	string place = analyzer->lookup(name);
	properties.push_back(Property{ 0, place });
	properties.push_back(Property{ 0, "" });
}

/**
 * @brief 检查符号表，要求不出现重复定义
 * @param analyzer 语义分析器
 * @param properties
*/
void notlookup(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	analyzer->notlookup(unwrap(analyzer, properties[0]));
}

/**
 * @brief 往符号表添加新符号
 * @param analyzer 语义分析器
 * @param properties
*/
void enter(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	string name = unwrap(analyzer, properties[0]);
	string type = unwrap(analyzer, properties[1]);
	int width;

	if (properties[2].index < 0) {
		width = stoi(properties[2].property);
	}
	else {
		Symbol* symbol = analyzer->getSymbolFromStack(properties[2].index);
		NonTerminator* non_terminator = static_cast<NonTerminator*>(symbol);
		width = *((int*)non_terminator->getFields(properties[2].property));
	}

	analyzer->enter(name, type, width);
}

/**
 * @brief 新建一个符号表
 * @param analyzer 语义分析器
 * @param properties
*/
void mktable(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	analyzer->mktable();
}

/**
 * @brief 回溯一个符号表
 * @param analyzer 语义分析器
 * @param properties
*/
void bktable(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	analyzer->bktable();
}

/**
 * @brief 回填四元式
 * @param analyzer 语义分析器
 * @param properties
*/
void backpatch(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	Symbol* symbol = analyzer->getSymbolFromStack(properties[0].index);
	NonTerminator* des = static_cast<NonTerminator*>(symbol);
	symbol = analyzer->getSymbolFromStack(properties[1].index);
	NonTerminator* src = static_cast<NonTerminator*>(symbol);

	int index = *(int*)des->getFields(properties[0].property);
	int quad = *(int*)src->getFields(properties[1].property);
	analyzer->backpatch(index, to_string(quad));
}

/**
 * @brief 产生四元式
 * @param analyzer 语义分析器
 * @param properties 
*/
void emit(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	string op = unwrap(analyzer, properties[0]);
	string arg1 = unwrap(analyzer, properties[1]);
	string arg2 = unwrap(analyzer, properties[2]);
	string res = unwrap(analyzer, properties[3]);
	analyzer->emite(op, arg1, arg2, res);
}

/**
 * @brief 下一条四元式的位置
 * @param analyzer 语义分析器
 * @param properties 存放位置
*/
void nextstat(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	int result = analyzer->nextstat(stoi(properties[0].property));

	properties.push_back(Property{ 0, to_string(result) });
	properties.push_back(Property{ 1, "" });
}

unordered_map<string, void (*)(SemanticAnalyzer*, std::vector<Property>&)> functionsPoint = {
	{"newtemp", newtemp},
	{"lookup", lookup},
	{"backpatch", backpatch},
	{"enter", enter},
	{"mktable", mktable},
	{"bktable", bktable},
	{"emit", emit},
	{"nextstat", nextstat},
	{"notlookup", notlookup}
};