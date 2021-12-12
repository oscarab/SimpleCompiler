#include <iostream>
#include <iomanip>
#include "Lexer.h"
#include "Parser.h"

#include "Symbol.h"

void showHelp();

int main(int argc, char* argv[]) {
    if (argc < 3) {
        showHelp();
        return 0;
    }
    bool tokens_out = false, tree_out = false, table_out = false, analysis_step = false, analysis_out = false;
    for (int i = 1; i < argc - 1; i++) {
        if (std::strcmp(argv[i], "--help") == 0) {
            showHelp();
            return 0;
        }
        else if (std::strcmp(argv[i], "--tokens") == 0) {
            tokens_out = true;
        }
        else if (std::strcmp(argv[i], "--tree") == 0) {
            tree_out = true;
        }
        else if (std::strcmp(argv[i], "--table") == 0) {
            table_out = true;
        }
        else if (std::strcmp(argv[i], "--step") == 0) {
            analysis_step = true;
        }
        else {
            std::cout << "error parameter!" << std::endl;
            return 0;
        }
    }

    char* code_file = argv[argc - 1];

    // 创建LR(1)分析表
    Parser parser("Grammer.txt");
    parser.createTable();

    // 判断是否输出分析表到文件
    if (table_out) {
        std::ofstream fout("table.txt");
        parser.writeTable(fout);
        fout.close();
    }

    Lexical::Lexer lexer(code_file);

    std::ofstream fout("analysis.txt");
    std::ofstream code_out("IntermediateCode.txt");
    bool sucess = parser.analysis(&lexer, fout, code_out, analysis_step);
    fout.close();

    // 判断是否输出所有单词
    if (tokens_out && sucess) {
        std::ofstream fout("tokens.txt");
        lexer.writeTokens(fout);
        fout.close();
    }
    else {
        std::ofstream fout("tokens.txt", std::ios::out);
        fout.close();
    }

    // 判断是否输出语法树到文件
    if (tree_out && sucess) {
        std::ofstream fout("tree.txt");
        parser.getTree()->getHead()->write(fout, 0, ' ');
        fout.close();
    }
    else {
        std::ofstream fout("tree.txt", std::ios::out);
        fout.close();
    }

    std::cout << "finish!";
    return sucess;
}

void showHelp() {
    using namespace std;
    cout << "Welcome to use SimpleCompiler!" << endl << endl;

    cout << "USAGE:" << endl;
    cout << "scc.exe [options] <input code file>" << endl << endl;

    cout << "OPTIONS:" << endl;
    cout << left << setw(10) << "--help"  << "- Show help" << endl;
    cout << left << setw(10) << "--tokens"  << "- Output tokens to file" << endl;
    cout << left << setw(10) << "--tree"  << "- Output syntax tree to file" << endl;
    cout << left << setw(10) << "--table" << "- Output LR(1) analysis table to file" << endl;
    cout << left << setw(10) << "--step" << "- Single step analysis" << endl;
}