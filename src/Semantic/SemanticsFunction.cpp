#include <vector>
#include <unordered_map>
#include "Semantic/SemanticAction.h"
#include "Semantic/SemanticAnalyzer.h"
#include "Parser/Symbol.h"

using namespace std;

/**
 * @brief ����װ
 * @param analyzer 
 * @param property 
 * @return ����װ�õ��ַ���
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
* @brief ͨ���ַ�������ʽ��ȡ������ָ��ĵ�ַ
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
 * @brief ��������ʱ����
 * @param analyzer ���������
 * @param properties ����½�����ʱ����
*/
void newtemp(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	properties.push_back(Property{ 0, analyzer->newtemp() });
	properties.push_back(Property{ 0, "" });
}

/**
 * @brief �����ű����˷����Ƿ�������
 * @param analyzer ���������
 * @param properties ��Ÿ÷��ŵ���Ե�ַ
*/
void lookup(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	string name = unwrap(analyzer, properties[0]);
	string place = analyzer->lookup(name);
	properties.push_back(Property{ 0, place });
	properties.push_back(Property{ 0, "" });
}

/**
 * @brief �����ű����˹����Ƿ�������
 * @param analyzer ���������
 * @param properties
*/
void lookupproc(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	string name = unwrap(analyzer, properties[0]);
	string type = properties.size() > 1 ? unwrap(analyzer, properties[1]) : "";
	analyzer->lookupproc(name, type);
}

/**
 * @brief ����Ƿ���main����
 * @param analyzer ���������
 * @param properties
*/
void checkmain(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	analyzer->checkmain();
}

/**
 * @brief �����ű�Ҫ�󲻳����ظ�����
 * @param analyzer ���������
 * @param properties
*/
void notlookup(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	analyzer->notlookup(unwrap(analyzer, properties[0]));
}

/**
 * @brief �����ű�����·���
 * @param analyzer ���������
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
 * @brief �����ű�����¹���
 * @param analyzer ���������
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
 * @brief �½�һ�����ű�
 * @param analyzer ���������
 * @param properties
*/
void mktable(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	analyzer->mktable();
}

/**
 * @brief ����һ�����ű�
 * @param analyzer ���������
 * @param properties
*/
void bktable(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	analyzer->bktable();
}

/**
 * @brief ������Ԫʽ
 * @param analyzer ���������
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
 * @brief ������Ԫʽ
 * @param analyzer ���������
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
 * @brief ��һ����Ԫʽ��λ��
 * @param analyzer ���������
 * @param properties ���λ��
*/
void nextstat(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	int result = analyzer->nextstat(stoi(properties[0].property));

	properties.push_back(Property{ 0, to_string(result) });
	properties.push_back(Property{ 1, "" });
}

/**
 * @brief ���Ҹñ�������������
 * @param analyzer ���������
 * @param properties ����
*/
void lookuptype(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	string name = unwrap(analyzer, properties[0]);

	properties.push_back(Property{ 0, analyzer->lookuptype(name) });
	properties.push_back(Property{ 0, "" });
}

/**
 * @brief ��������Ƿ��������
 * @param analyzer ���������
 * @param properties
*/
void checktype(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	string t1 = unwrap(analyzer, properties[0]);
	string t2 = unwrap(analyzer, properties[1]);

	analyzer->checktype(t1, t2);
}

/**
 * @brief ������ʽ����
 * @param analyzer ���������
 * @param properties
*/
void addpara(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	string t = unwrap(analyzer, properties[0]);

	analyzer->addpara(t);
}

/**
 * @brief ����ʵ��
 * @param analyzer ���������
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
 * @brief ��������������
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
 * @brief �ϲ�����������
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
 * @brief ����ά��
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
 * @brief ����һ�����鵽���ű�
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
 * @brief ���������ƫ�Ƶ�ַ
 * @param analyzer
 * @param properties
*/
void computearr(SemanticAnalyzer* analyzer, vector<Property>& properties) {
	string array = unwrap(analyzer, properties[0]);		// ������
	string place = unwrap(analyzer, properties[1]);		// ��ǰά�ȱ��ʽ
	string dim = unwrap(analyzer, properties[2]);		// ��ǰά����
	string plus = unwrap(analyzer, properties[3]);		// Ҫ���ϵĳ�ʼƫ����

	string num = analyzer->computearr(array, std::stoi(dim));
	string temp = analyzer->newtemp();
	if (plus == "0" && num == "0") {
		analyzer->emite(":=", place, "_", temp);
		properties.push_back(Property{ 0, temp });
		properties.push_back(Property{ 0, "" });
		return;
	}
	if (plus != "0") {
		// ��һ��ά�Ȳ��ü�
		analyzer->emite("+", plus, place, temp);
		if (num == "0") {
			properties.push_back(Property{ 0, temp });
			properties.push_back(Property{ 0, "" });
		}
	}
	// ָ��ά�ȼ�һ
	NonTerminator* pointer = static_cast<NonTerminator*>(analyzer->getSymbolFromStack(properties[2].index));
	int* p = (int*)pointer->getFields(properties[2].property);
	*p = *p + 1;
	if (num != "0") {
		// ���һ��ά�Ȳ��ó�
		if (plus == "0") {	
			analyzer->emite("*", place, num, temp);
			properties.push_back(Property{ 0, temp });
			properties.push_back(Property{ 0, "" });
		}
		else {
			// ֮���ά�ȣ�����ʹ��֮ǰ�����õ���ʱ��������
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