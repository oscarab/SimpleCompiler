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
 * @brief ����һ���²���ʽ
 * @param psymbol ����ʽ�Ҳ��ķ��Ŵ�
*/
void NonTerminator::insertProduction(SymbolChain& psymbol) {
	production.push_back(psymbol);	// ���²���ʽ
	psymbol.clear();				//׼��������һ������ʽ
}

/**
 * @brief ���ս�����
 * @param out ���Դ
 * @param level ����
*/
void NonTerminator::write(Log* log, int level) {
	tab(log, level);
	log->logln("\"type\": \"NonTerminator\",");
	tab(log, level);
	log->logln("\"attribute\": \"" + name + "\",");
}

/**
 * @brief �ս�����
 * @param out ���Դ
 * @param level ����
*/
void Terminator::write(Log* log, int level) {
	token.write(log, level, false);
}

void Terminator::setupReflect() { }

/**
 * @brief ���淴����Ҫ����Ϣ
*/
void NonTerminator::setupReflect() {
	reflectMap["name"] = ReflectItem{ "string", (char*)&name };
	reflectMap["production"] = ReflectItem{ "vector<SymbolChain>", (char*)&production };
}

/**
 * @brief �����ȡ��Ա����
 * @param fields_name ��Ա��������
 * @return ��Ա������ָ��
*/
char* ReflectBase::getFields(std::string fields_name) {
	return reflectMap[fields_name].data;
}

/**
 * @brief �����ȡ��Ա����������
 * @param fields_name ��Ա��������
 * @return ����
*/
std::string ReflectBase::getFieldType(std::string fields_name) {
	return reflectMap[fields_name].type;
}

/**
 * @brief �����޸ĳ�Ա����
 * @param fields_name ��Ա��������
 * @param data Ҫ��Ϊ������
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