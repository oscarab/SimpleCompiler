#include "Lexer.h"

using namespace LexicalAnalysis;

Lexer::Lexer(char* fileName) {
	idTable = std::vector<std::string>(126);
	constantTable = std::vector<double>(126);

	codeReader = Reader(fileName);
	scanner = Scanner(&idTable, &constantTable);
}

void Lexer::run() {

}