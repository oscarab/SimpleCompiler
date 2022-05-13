#include "Lexer/Token.h"
#include "Parser/Symbol.h"
#include "Output/Log.h"

extern void tab(Log* log, int level);

Terminator::Terminator(Token _token) : token(_token) {
}

Token Terminator::getToken() const {
	return token;
}

bool Terminator::isEnd() const {
	return true;
}

NonTerminator::NonTerminator(String str) {
	name = str;
	setupReflect();
}

String NonTerminator::getName() const {
	return name;
}

bool NonTerminator::isEnd() const {
	return false;
}

std::vector<SymbolChain>* NonTerminator::getProductions() {
	return &production;
}

/**
 * @brief 插入一条新产生式
 * @param psymbol 产生式右部的符号串
*/
void NonTerminator::insertProduction(SymbolChain& psymbol) {
	production.push_back(psymbol);	// 更新产生式
	psymbol.clear();				//准备读入下一条产生式
}

/**
 * @brief 非终结符输出
 * @param out 输出源
 * @param level 缩进
*/
void NonTerminator::write(Log* log, int level) {
	tab(log, level);
	log->logln("\"type\": \"NonTerminator\",");
	tab(log, level);
	log->logln("\"attribute\": \"" + name + "\",");
}

/**
 * @brief 终结符输出
 * @param out 输出源
 * @param level 缩进
*/
void Terminator::write(Log* log, int level) {
	token.write(log, level, false);
}

void Terminator::setupReflect() { }

/**
 * @brief 储存反射需要的信息
*/
void NonTerminator::setupReflect() {
	reflectMap["name"] = ReflectItem{ "string", (char*)&name };
	reflectMap["production"] = ReflectItem{ "vector<SymbolChain>", (char*)&production };
}

/**
 * @brief 反射获取成员变量
 * @param fields_name 成员变量名字
 * @return 成员变量的指针
*/
char* ReflectBase::getFields(std::string fields_name) {
	return reflectMap[fields_name].data;
}

/**
 * @brief 反射获取成员变量的类型
 * @param fields_name 成员变量名字
 * @return 类型
*/
std::string ReflectBase::getFieldType(std::string fields_name) {
	return reflectMap[fields_name].type;
}

/**
 * @brief 反射修改成员变量
 * @param fields_name 成员变量名字
 * @param data 要改为的数据
*/
void ReflectBase::setFields(std::string fields_name, char* data) {
	ReflectItem item = reflectMap[fields_name];
	char* place = item.data;
	std::string type = item.type;

	if (type == "int") {
		*((int*)place) = *((int*)data);
	}
	else if (type == "string") {
		*((std::string*)place) = *((std::string*)data);
	}
	else if (type == "vector") {
		*((std::vector<int>*)place) = *((std::vector<int>*)data);
	}
}

bool Symbol::operator==(const Symbol& symbol) const {
	bool end = isEnd();

	if (symbol.isEnd() != end) return false;
	if (end) {
		Terminator* tor1 = static_cast<Terminator*>(const_cast<Symbol*>(this));
		Terminator* tor2 = static_cast<Terminator*>(const_cast<Symbol*>(&symbol));
		return tor1->getToken() == tor2->getToken();
	}
	else {
		NonTerminator* nontor1 = static_cast<NonTerminator*>(const_cast<Symbol*>(this));
		NonTerminator* nontor2 = static_cast<NonTerminator*>(const_cast<Symbol*>(&symbol));
		return nontor1->getName() == nontor2->getName();
	}
}