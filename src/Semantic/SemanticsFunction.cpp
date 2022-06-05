#include <vector>
#include <unordered_map>
#include "Semantic/SemanticAction.h"
#include "Semantic/SemanticAnalyzer.h"
#include "Parser/Symbol.h"

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

/*
* @brief 通过字符串的形式获取接下来指令的地址
*/
String convertToPointer(SemanticAnalyzer* analyzer, String text) {
	if (text == "next1") {
		return to_string(analyzer->nextstat(1));
	}
	else if (text == "next2") {
		return to_string(analyzer->nextstat(2));
	}
	else if (text == "next3") {
		return to_string(analyzer->nextstat(3));
	}
	return text;
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
 * @brief 检查符号表，看此符号是否被声明了
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
 * @brief 检查符号表，看此过程是否被声明了
 * @param analyzer 语义分析器
 * @param properties
*/
void lookupproc(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	string name = unwrap(analyzer, properties[0]);
	string type = properties.size() > 1 ? unwrap(analyzer, properties[1]) : "";
	analyzer->lookupproc(name, type);
}

/**
 * @brief 检查是否有main函数
 * @param analyzer 语义分析器
 * @param properties
*/
void checkmain(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	analyzer->checkmain();
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
 * @brief 往符号表添加新过程
 * @param analyzer 语义分析器
 * @param properties
*/
void enterproc(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	string name = unwrap(analyzer, properties[0]);
	string type = unwrap(analyzer, properties[1]);
	string position = unwrap(analyzer, properties[2]);
	string arg = unwrap(analyzer, properties[3]);
	analyzer->enterproc(name, type, position, arg);
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
	string quad = unwrap(analyzer, properties[1]);

	quad = convertToPointer(analyzer, quad);

	if (des->getFieldType(properties[0].property) == "vector") {
		vector<int>* indices = (vector<int>*)des->getFields(properties[0].property);

		for (int idx : *indices) {
			analyzer->backpatch(idx, quad);
		}
	}
	else {
		int* idx = (int*)des->getFields(properties[0].property);
		analyzer->backpatch(*idx, quad);
	}
	
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

	if (op == "j") res = convertToPointer(analyzer, res);
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

/**
 * @brief 查找该变量或函数的类型
 * @param analyzer 语义分析器
 * @param properties 类型
*/
void lookuptype(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	string name = unwrap(analyzer, properties[0]);

	properties.push_back(Property{ 0, analyzer->lookuptype(name) });
	properties.push_back(Property{ 0, "" });
}

/**
 * @brief 检查类型是否可以运算
 * @param analyzer 语义分析器
 * @param properties
*/
void checktype(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	string t1 = unwrap(analyzer, properties[0]);
	string t2 = unwrap(analyzer, properties[1]);

	analyzer->checktype(t1, t2);
}

/**
 * @brief 保存形式参数
 * @param analyzer 语义分析器
 * @param properties
*/
void addpara(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	string t = unwrap(analyzer, properties[0]);

	analyzer->addpara(t);
}

/**
 * @brief 保存实参
 * @param analyzer 语义分析器
 * @param properties
*/
void addarg(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	NonTerminator* des = static_cast<NonTerminator*>(analyzer->getSymbolFromStack(properties[0].index));
	NonTerminator* src = static_cast<NonTerminator*>(analyzer->getSymbolFromStack(properties[1].index));

	String str1 = *(String*)src->getFields(properties[1].property);
	String str2 = *(String*)des->getFields(properties[0].property);
	des->setFields(properties[0].property, (char*)&(str2 + str1 + ";"));
}

/**
 * @brief 创建真假入口链表
 * @param analyzer 
 * @param properties 
*/
void makelist(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	NonTerminator* t = static_cast<NonTerminator*>(analyzer->getSymbolFromStack(properties[0].index));
	vector<int>* list = (vector<int>*) t->getFields(properties[0].property);
	int result = analyzer->nextstat(stoi(properties[1].property));

	list->push_back(result);
}

/**
 * @brief 合并真假入口链表
 * @param analyzer 
 * @param properties 
*/
void merge(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	NonTerminator* des = static_cast<NonTerminator*>(analyzer->getSymbolFromStack(properties[0].index));
	NonTerminator* arg1 = static_cast<NonTerminator*>(analyzer->getSymbolFromStack(properties[1].index));
	NonTerminator* arg2 = static_cast<NonTerminator*>(analyzer->getSymbolFromStack(properties[2].index));
	vector<int>* des_list = (vector<int>*) des->getFields(properties[0].property);
	vector<int>* list1 = (vector<int>*) arg1->getFields(properties[1].property);
	vector<int>* list2 = (vector<int>*) arg2->getFields(properties[2].property);

	des_list->insert(des_list->end(), list1->begin(), list1->end());
	des_list->insert(des_list->end(), list2->begin(), list2->end());
}

/**
 * @brief 加入维度
 * @param analyzer
 * @param properties
*/
void adddim(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	NonTerminator* arg1 = static_cast<NonTerminator*>(analyzer->getSymbolFromStack(properties[0].index));
	vector<int>* dim_list = (vector<int>*) arg1->getFields(properties[0].property);
	string place = unwrap(analyzer, properties[1]);
	string type = unwrap(analyzer, properties[2]);

	if (type != "int") {
		std::cout << "dim type must be int";
		exit(0);
	}
	if (!std::isdigit(place[0]) || std::stoi(place) <= 0) {
		std::cout << "dim must be positive";
		exit(0);
	}
	dim_list->push_back(std::stoi(place));
}

/**
 * @brief 加入一个数组到符号表
 * @param analyzer
 * @param properties
*/
void enterarray(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	NonTerminator* arg3 = static_cast<NonTerminator*>(analyzer->getSymbolFromStack(properties[2].index));

	string name = unwrap(analyzer, properties[0]);
	string type = unwrap(analyzer, properties[1]);
	vector<int>* dims = (vector<int>*) arg3->getFields(properties[2].property);
	int dim = dims->size();
	int width = 4;
	for (int i = 0; i < dim; i++) {
		width *= dims->at(i);
	}

	analyzer->enterarray(name, type, *dims, width, dim);
}

/**
 * @brief 计算数组的偏移地址
 * @param analyzer
 * @param properties
*/
void computearr(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	string array = unwrap(analyzer, properties[0]);		// 数组名
	string place = unwrap(analyzer, properties[1]);		// 当前维度表达式
	string dim = unwrap(analyzer, properties[2]);		// 当前维度数
	string plus = unwrap(analyzer, properties[3]);		// 要加上的初始偏移量

	string num = analyzer->computearr(array, std::stoi(dim));
	string temp = analyzer->newtemp();
	if (plus == "0" && num == "0") {
		analyzer->emite(":=", place, "_", temp);
		properties.push_back(Property{ 0, temp });
		properties.push_back(Property{ 0, "" });
		return;
	}
	if (plus != "0") {
		// 第一个维度不用加
		analyzer->emite("+", plus, place, temp);
		if (num == "0") {
			properties.push_back(Property{ 0, temp });
			properties.push_back(Property{ 0, "" });
		}
	}
	// 指向维度加一
	NonTerminator* pointer = static_cast<NonTerminator*>(analyzer->getSymbolFromStack(properties[2].index));
	int* p = (int*)pointer->getFields(properties[2].property);
	*p = *p + 1;
	if (num != "0") {
		// 最后一个维度不用乘
		if (plus == "0") {	
			analyzer->emite("*", place, num, temp);
			properties.push_back(Property{ 0, temp });
			properties.push_back(Property{ 0, "" });
		}
		else {
			// 之后的维度，继续使用之前创建好的临时变量计算
			string res = analyzer->newtemp();
			analyzer->emite("*", temp, num, res);

			properties.push_back(Property{ 0, res });
			properties.push_back(Property{ 0, "" });
		}
	}
}

unordered_map<string, void (*)(SemanticAnalyzer*, std::vector<Property>&)> functionsPointer = {
	{"newtemp", newtemp},
	{"lookup", lookup},
	{"backpatch", backpatch},
	{"enter", enter},
	{"mktable", mktable},
	{"bktable", bktable},
	{"emit", emit},
	{"nextstat", nextstat},
	{"notlookup", notlookup},
	{"enterproc", enterproc},
	{"lookupproc", lookupproc},
	{"checkmain", checkmain},
	{"lookuptype", lookuptype},
	{"checktype", checktype},
	{"addpara", addpara},
	{"addarg", addarg},
	{"makelist", makelist},
	{"merge", merge},
	{"adddim", adddim},
	{"enterarray", enterarray},
	{"computearr", computearr}
};