#include <iostream>
#include "Lexer.h"
#include "Parser.h"

int main()
{
    Parser parser("Grammer.txt");
    parser.createTable();

    std::ofstream fout("table.txt");
    parser.writeTable(fout);
    fout.close();

    Lexical::Lexer lexer("code.txt");
    lexer.run();
    parser.analysis(&lexer);
    fout = std::ofstream("tree.txt");
    parser.getTree()->getHead()->write(fout, 0, ' ');
    fout.close();

    std::cout << "finish!";
    return 0;
}