#include "AttributeSymbol.h"
#include "Token.h"

extern std::vector<std::string> idTable;
extern std::vector<double> constantTable;
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
		name = std::to_string(constantTable[token.getIndex()]);
	}
}

void LeafSymbol::setupReflect() {
	reflectMap["name"] = ReflectItem{ "string", (char*)&name };
	reflectMap["place"] = ReflectItem{ "string", (char*)&place };
}

void VariableSymbol::setupReflect() {
	reflectMap["place"] = ReflectItem{ "string", (char*)&place };
	reflectMap["type"] = ReflectItem{ "string", (char*)&type };
	reflectMap["width"] = ReflectItem{ "int", (char*)&width };
}

void OperatorSymbol::setupReflect() {
	reflectMap["op"] = ReflectItem{ "string", (char*)&op };
}

void ControlSymbol::setupReflect() {
	reflectMap["truelist"] = ReflectItem{ "int", (char*)&truelist };
	reflectMap["falselist"] = ReflectItem{ "int", (char*)&falselist };
}

void EpsilonSymbol::setupReflect() {
	reflectMap["quad"] = ReflectItem{ "int", (char*)&quad };
	reflectMap["truelist"] = ReflectItem{ "int", (char*)&truelist };
}