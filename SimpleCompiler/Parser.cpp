#include "Parser.h"
#include "Lexer.h"
#include <conio.h>
#include <iostream>
#include <iomanip>

extern void tab(std::ostream& out, int level);
extern int getWidth(int number, std::string str);
extern std::string convertToString(Token& token);

/**
 * @brief LR(1)语法分析器构造函数
 * @param fileName 文法文件名
*/
Parser::Parser(const char* fileName) : machine(fileName) {
	machine.create();
}

/**
 * @brief 创建LR(1)分析表
*/
void Parser::createTable() {
	std::vector<State>* states = machine.getStates();
	Transfer* transfer = machine.getTransfer();
	Grammer* grammer = machine.getGrammer();
	int ssize = transfer->size();

	// 遍历所有状态
	for (int i = 0; i < ssize; i++) {
		SymMapInt& stateTrans = (*transfer)[i];
		std::unordered_map<Symbol*, Action> tuple;
		State& state = (*states)[i];

		// 遍历每一个项目
		for (auto iter = state.begin(); iter != state.end(); iter++) {
			Item item = *iter;
			Symbol* sym = NULL;
			Action action;
			
			if (item.isAccept()) {
				// 接受
				action.accept = true;
				tuple[grammer->getSymbol("#")] = action;
			}
			else if (sym = item.isMoveIn()) {
				// 移入
				action.action = sym->isEnd();
				action.go = stateTrans[sym];
				tuple[sym] = action;
			}
			else if (item.isReduction()) {
				// 归约
				action.action = true;
				action.reduction = true;
				action.product = item.getProduction();
				int base = machine.getGrammer()->getProductionCount(action.product.symbolPoint);
				action.go = base + action.product.productionIndex;
				tuple[item.getForward()] = action;
			}
		}

		table.push_back(tuple);
	}
}

/**
 * @brief 使用LR(1)进行语法分析
 * @param lexer 词法分析器
 * @return 是否分析成功
*/
bool Parser::analysis(Lexical::Lexer* lexer, std::ostream& out, bool step) {
	// 获取Token流
	if (!lexer->run()) return false;
	std::vector<Token>* tokens = lexer->getTokens();
	int len = tokens->size();
	Grammer* grammer = machine.getGrammer();
	stateStack.push_back(0);
	symbolStack.push_back(grammer->getSymbol(String("#")));

	for (int i = 0; i < len; i++) {
		int nowState = stateStack[stateStack.size() - 1];
		Token token = (*tokens)[i];
		int index = token.getIndex();
		Terminator terminator(token);

		// 单步分析的信息输出
		if (step) {
			system("cls");
			writeStack(std::cout);
			std::cout << std::endl << "Now at: " << std::endl;
			terminator.write(std::cout, 0);
			std::cout << std::endl << "Action: " << std::endl;
		}
		writeStack(out);
		out << std::endl << "Now at: " << std::endl;
		terminator.write(out, 0);
		out << std::endl << "Action: " << std::endl;

		// 遇到可能错误的语法
		if (table[nowState].count(&terminator) == 0) {
			std::cout << "syntax error!" << std::endl;
			out << "syntax error!" << std::endl;
			return false;
		}

		// 需要采取的动作
		Action action = table[nowState][&terminator];
		
		if (action.accept) {
			std::cout << "Accept!" << std::endl;
			out << "Accept!" << std::endl;
			return true;
		}
		else if (action.reduction) {
			// 归约
			Production product = action.product;
			SymbolChain& product_str = (*product.symbolPoint->getProductions())[product.productionIndex];
			
			int pop_cnt = product_str.size();
			if (product_str[0]->isEnd() && static_cast<Terminator*>(product_str[0])->getToken().isEmpty()) {
				pop_cnt = 0;
			}
			int reduce_cnt = pop_cnt;

			tree.construct(product.symbolPoint, pop_cnt);

			while (pop_cnt--) {
				symbolStack.pop_back();
				stateStack.pop_back();
			}

			nowState = stateStack[stateStack.size() - 1];
			int dest = table[nowState][product.symbolPoint].go;

			symbolStack.push_back(product.symbolPoint);
			stateStack.push_back(dest);
			i--;

			if (step) {
				std::cout << "Reduce the " << reduce_cnt << " characters at the top of the stack" << std::endl;
				std::cout << "Goto state " << dest;
				_getch();
			}
			out << "Reduce the " << reduce_cnt << " characters at the top of the stack" << std::endl;
			out << "Goto state " << dest << std::endl;
		}
		else {
			// 移入
			token.setIndex(index);
			Symbol* read_sym = new Terminator(token);
			
			tree.insert(read_sym);
			symbolStack.push_back(read_sym);
			stateStack.push_back(action.go);

			if (step) {
				std::cout << "Push the current symbol onto the symbol stack" << std::endl;
				std::cout << "Goto state " << action.go;
				_getch();
			}
			out << "Push the current symbol onto the symbol stack" << std::endl;
			out << "Goto state " << action.go << std::endl;

		}
	}
	return false;
}

/**
 * @brief 输出表格
 * @param out 输出源
*/
void Parser::writeTable(std::ostream& out) {
	using namespace std;
	vector<State>* states = machine.getStates();
	int size = states->size();			// 状态数量

	// 筛选终结符和非终结符
	std::vector<Terminator*> terminator;
	std::vector<NonTerminator*> non_terminator;
	SymbolChain* symbols = machine.getGrammer()->getSymbols();
	int sym_cnt = symbols->size();
	for (int i = 0; i < sym_cnt; i++) {
		if ((*symbols)[i]->isEnd()) {
			terminator.push_back(static_cast<Terminator*>((*symbols)[i]));
		}
		else {
			non_terminator.push_back(static_cast<NonTerminator*>((*symbols)[i]));
		}
	}

	/* 输出表格 */

	// 计算列宽
	int whole_w = 0;
	int term_cnt = terminator.size(), non_term_cnt = non_terminator.size();
	vector<int> width;
	width.push_back(getWidth(size, "状态") + 4);
	for (Terminator* sym : terminator) {
		Token token = sym->getToken();
		int w = getWidth(size, convertToString(token)) + 4;
		width.push_back(w);
		whole_w += w;
	}
	for (NonTerminator* sym : non_terminator) {
		int w = getWidth(size, sym->getName()) + 4;
		width.push_back(w);
		whole_w += w;
	}
	whole_w += term_cnt + non_term_cnt + 2;

	// 表头
	out << "|";
	out << setw(width[0]) << "状态";
	out << "|";
	for (int i = 0; i < term_cnt; i++) {
		Token token = terminator[i]->getToken();
		out << setw(width[i + 1]) << convertToString(token);
		out << "|";
	}
	for (int i = 0; i < non_term_cnt; i++) {
		out << setw(width[term_cnt + i + 1]) << non_terminator[i]->getName();
		out << "|";
	}
	out << std::endl;

	// 各个状态
	for (int i = 0; i < size; i++) {
		out << "|";
		out << setw(width[0]) << i;
		out << "|";
		for (int j = 0; j < term_cnt + non_term_cnt; j++) {
			Symbol* sym = j < term_cnt? (Symbol*) terminator[j] : non_terminator[j - term_cnt];
			auto iter = table[i].find(sym);
			out << setw(width[j + 1]);

			if (iter != table[i].end()) {
				Action action = (*iter).second;
				out << action.toString();
			}
			else {
				out << " ";
			}
			out << "|";
		}
		out << std::endl;
	}
}

/**
 * @brief 栈 内容的显示
 * @param out 输出
*/
void Parser::writeStack(std::ostream& out) {
	using namespace std;
	int sym_cnt = symbolStack.size();
	int state_cnt = stateStack.size();
	int stack_h = sym_cnt > state_cnt ? sym_cnt : state_cnt;
	stack_h += 2;

	for (int i = stack_h - 1; i >= 0; i--) {
		out << right << setw(5) << "|";
		if (i < sym_cnt) {
			if (symbolStack[i]->isEnd()) {
				Token token = static_cast<Terminator*>(symbolStack[i])->getToken();
				out << left << setw(15) << convertToString(token);
			}
			else {
				out << left << setw(15) << static_cast<NonTerminator*>(symbolStack[i])->getName();
			}
		}
		else {
			out << setw(15) << " ";
		}
		out << left << setw(5) << "|";

		out << right << setw(5) << "|";
		if (i < state_cnt) {
			out << left << setw(15) << stateStack[i];
		}
		else {
			out << left << setw(15) << " ";
		}
		out << left << setw(5) << "|";
		out << endl;
	}

	out << setw(5) << " ";
	for (int i = 0; i < 15; i++) {
		out << "-";
	}
	out << setw(5) << " ";
	out << setw(5) << " ";
	for (int i = 0; i < 15; i++) {
		out << "-";
	}
	out << endl;

	out << setw(5) << " " << setw(15) << "Symbol Stack";
	out << setw(5) << " ";
	out << setw(5) << " " << setw(15) << "State Stack" << endl;
}