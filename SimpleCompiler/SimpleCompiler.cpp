#include <iostream>
#include "Lexer.h"
#include "Grammer.h"

int main()
{
    //Lexical::Lexer lexer("E:/编译原理/code.txt");
    //lexer.run();

    //std::vector<Token>* tokens = lexer.getTokens();
    //lexer.show();

    Grammer grammer("E:/编译原理/grammer.txt");
    grammer.solveCanEmpty();
    grammer.solveFirst();

    std::cout << "sdadw";
    return 0;
}