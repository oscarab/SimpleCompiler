#include <iostream>
#include "Lexer.h"
#include "Parser.h"

int main()
{
    Parser parser("Grammer.txt");
    parser.createTable();

    parser.analysis("code.txt");
    std::cout << "sdadw";
    return 0;
}