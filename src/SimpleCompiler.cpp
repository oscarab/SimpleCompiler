#include <iostream>
#include <iomanip>
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "Parser/Symbol.h"
#include "Output/Output.h"

void showHelp();
Output output;

int main(int argc, char* argv[]) {
    if (argc < 2) {
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
    output.add("table.txt");
    output.add("analysis.txt");
    output.add("IntermediateCode.txt");
    output.add("tokens.txt");
    output.add("tree.txt");
    output.setStep(analysis_step);

    // 创建LR(1)分析表
    Parser parser("Grammer.txt");
    parser.createTable();

    // 判断是否输出分析表到文件
    if (table_out) {
        parser.writeTable();
    }

    Lexical::Lexer lexer(code_file);

    bool sucess = parser.analysis(&lexer);

    parser.optimize();
    parser.generate();

    // 判断是否输出所有单词
    if (tokens_out && sucess) {
        lexer.writeTokens();
    }

    // 判断是否输出语法树到文件
    if (tree_out && sucess) {
        parser.getTree()->getHead()->write(output[4], 0, ' ');
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