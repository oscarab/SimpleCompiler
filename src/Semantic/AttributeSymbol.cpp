#include "Semantic/AttributeSymbol.h"
#include "Lexer/Token.h"

extern std::vector<std::string> idTable;
extern std::vector<std::string> constantTable;
extern const char* TokenAttrStr[];

LeafSymbol::LeafSymbol(Token token) : Terminator(token){
	if (token.getType() == TokenType::ID) {
		name = idTable[token.getIndex()];
	}
	else if (token.getType() == TokenType::OPERATOR) {
		name = TokenAttrStr[(int) token.getAttribute()];
	}
	else if (token.getType() == TokenType::KEY_WORD) {
		name = TokenAttrStr[(int)token.getAttribute()];
	}
	else if (token.getType() == TokenType::CONSTANT) {
		String number = constantTable[token.getIndex()];
		name = number;
		type = number.find(".") == number.npos ? "int" : "float";
	}
}

void LeafSymbol::setupReflect() {
	reflectMap["name"] = ReflectItem{ "string", (char*)&name };
	reflectMap["place"] = ReflectItem{ "string", (char*)&place };
	reflectMap["type"] = ReflectItem{ "string", (char*)&type };
}

void VariableSymbol::setupReflect() {
	reflectMap["place"] = ReflectItem{ "string", (char*)&place };
	reflectMap["type"] = ReflectItem{ "string", (char*)&type };
	reflectMap["array"] = ReflectItem{ "string", (char*)&array };
	reflectMap["width"] = ReflectItem{ "int", (char*)&width };
	reflectMap["truelist"] = ReflectItem{ "vector", (char*)&truelist };
	reflectMap["falselist"] = ReflectItem{ "vector", (char*)&falselist };
}

void OperatorSymbol::setupReflect() {
	reflectMap["op"] = ReflectItem{ "string", (char*)&op };
}

void EpsilonSymbol::setupReflect() {
	reflectMap["quad"] = ReflectItem{ "int", (char*)&quad };
	reflectMap["nextlist"] = ReflectItem{ "int", (char*)&nextlist };
}