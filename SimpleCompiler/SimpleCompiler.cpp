#include <iostream>
#include "Lexer.h"

int main()
{
    Lexical::Lexer lexer("E:/编译原理/code.txt");
    lexer.run();

    std::vector<Token> tokens = lexer.getTokens();
    lexer.show();

    return 0;
}