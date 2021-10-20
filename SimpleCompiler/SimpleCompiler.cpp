#include <iostream>
#include "Lexer.h"

int main()
{
    LexicalAnalysis::Lexer lexer("E:/编译原理/code.txt");
    lexer.run();
    std::vector<LexicalAnalysis::Token> tokens = lexer.getTokens();
    for (LexicalAnalysis::Token token : tokens) {
        std::cout << token;
    }
}
