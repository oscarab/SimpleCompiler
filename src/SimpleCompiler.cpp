#include <iostream>
#include <iomanip>
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "Parser/Symbol.h"
#include "Output/Output.h"
#include "Output/Log.h"
#include "Output/Exception.h"

void showHelp();

int main(int argc, char* argv[]) {
    Output& output = *Output::getInstance();
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
    FileLog::factoryBuild("analysis.txt", "analysis.txt");
    FileLog::factoryBuild("IntermediateCode.txt", "IntermediateCode.txt");
    FileLog::factoryBuild("MipsAsm.asm", "MipsAsm.asm");
    if (tokens_out) FileLog::factoryBuild("tokens.txt", "tokens.txt");
    if (table_out) FileLog::factoryBuild("table.txt", "table.txt");
    if (tree_out) FileLog::factoryBuild("tree.txt", "tree.txt");
    output.setStep(analysis_step);

    try {
        // 创建LR(1)分析表
        Parser parser("Grammer.txt");
        output.sendMessage("start build LR(1) table...");
        parser.createTable();

        // 判断是否输出分析表到文件
        if (table_out) {
            parser.writeTable();
        }

        // 定义词法分析器
        Lexical::Lexer lexer(code_file);

        output.sendMessage("start analysis...");
        parser.analysis(&lexer);    // 生成中间代码
        output.sendMessage("start optimize...");
        parser.optimize();          // DAG优化
        output.sendMessage("start generate...");
        parser.generate();          // 生成目标代码

        // 判断是否输出所有单词
        if (tokens_out) {
            lexer.writeTokens();
        }

        // 判断是否输出语法树到文件
        if (tree_out) {
            Log* tree_log = FileLog::getInstance("tree.txt");
            parser.getTree()->getHead()->write(0, ' ');
        }

        output.sendMessage("Finish!");
    }
    catch (const CompilerException& e) {
        // 处理编译中遇到的异常
        output.sendMessage(e.what());
        Token token = e.getToken();
        output.sendMessage(
            "at line " + std::to_string(token.getRow()) + " charactor " + std::to_string(token.getColumn())
        );
    }
    
    return 0;
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