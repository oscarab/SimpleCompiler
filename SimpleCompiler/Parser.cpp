#include "Parser.h"
#include "Lexer.h"
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
	int ssize = transfer->size();

	// 遍历所有状态
	for (int i = 0; i < ssize; i++) {
		SymMapInt& stateTrans = (*transfer)[i];
		std::unordered_map<Symbol, Action> tuple;
		State& state = (*states)[i];

		// 遍历每一个项目
		for (auto iter = state.begin(); iter != state.end(); iter++) {
			Item item = *iter;
			Symbol* sym = NULL;
			Action action;
			
			if (item.isAccept()) {
				// 接受
				action.accept = true;
				tuple[Symbol(Token(TokenType::END, TokenAttribute::None))] = action;
			}
			else if (sym = item.isMoveIn()) {
				// 移入
				action.action = sym->isEnd();
				action.go = stateTrans[*sym];
				tuple[*sym] = action;
			}
			else if (item.isReduction()) {
				// 归约
				action.action = true;
				action.reduction = true;
				action.prod = item.getProduction();
				int base = machine.getGrammer()->getProductionCount(action.prod.symbolIndex);
				action.go = base + action.prod.productionIndex;
				tuple[*item.getForward()] = action;
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
bool Parser::analysis(Lexical::Lexer* lexer) {
	// 获取Token流
	std::vector<Token>* tokens = lexer->getTokens();
	int len = tokens->size();
	Grammer* grammer = machine.getGrammer();
	stateStack.push_back(0);

	for (int i = 0; i < len; i++) {
		int nowState = stateStack[stateStack.size() - 1];
		Token token = (*tokens)[i];
		token.setDefaultIndex();
		Symbol symbol(token);

		// 遇到可能错误的语法
		if (table[nowState].count(symbol) == 0) {
			return false;
		}

		// 需要采取的动作
		Action action = table[nowState][symbol];
		
		if (action.accept) {
			return true;
		}
		else if (action.reduction) {
			// 归约
			Production product = action.prod;
			int pop_cnt = (*product.symbolPoint->getProductions())[product.productionIndex].size();

			tree.construct(product.symbolPoint, pop_cnt);

			while (pop_cnt--) {
				symbolStack.pop_back();
				stateStack.pop_back();
			}

			nowState = stateStack[stateStack.size() - 1];
			int dest = table[nowState][*product.symbolPoint].go;

			symbolStack.push_back(product.symbolPoint);
			stateStack.push_back(dest);
			i--;
		}
		else {
			// 移入
			Symbol* read_sym = grammer->getSymbol(&symbol);
			
			tree.insert(read_sym);
			symbolStack.push_back(read_sym);
			stateStack.push_back(action.go);
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
	PSymbol terminator, non_terminator;
	PSymbol* symbols = machine.getGrammer()->getSymbols();
	int sym_cnt = symbols->size();
	for (int i = 0; i < sym_cnt; i++) {
		if ((*symbols)[i]->isEnd()) {
			terminator.push_back((*symbols)[i]);
		}
		else {
			non_terminator.push_back((*symbols)[i]);
		}
	}

	/* 输出表格 */

	// 计算列宽
	int whole_w = 0;
	int term_cnt = terminator.size(), non_term_cnt = non_terminator.size();
	vector<int> width;
	width.push_back(getWidth(size, "状态") + 4);
	for (Symbol* sym : terminator) {
		Token token = sym->getToken();
		int w = getWidth(size, convertToString(token)) + 4;
		width.push_back(w);
		whole_w += w;
	}
	for (Symbol* sym : non_terminator) {
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
		for (int j = 0; j < term_cnt; j++) {
			Symbol* sym = terminator[j];
			auto iter = table[i].find(*sym);
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
		for (int j = 0; j < non_term_cnt; j++) {
			Symbol* sym = non_terminator[j];
			auto iter = table[i].find(*sym);
			out << setw(width[term_cnt + j + 1]);

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
 * @brief 获取语法树
 * @return 语法树
*/
SyntaxTree* Parser::getTree() {
	return &tree;
}

/**
 * @brief 语法树结点构造
 * @param sym 符号
*/
SyntaxNode::SyntaxNode(Symbol* sym) {
	symbol = sym;
}

/**
 * @brief 给结点增加孩子
 * @param node 孩子结点
*/
void SyntaxNode::addChild(SyntaxNode* node) {
	child.push_back(node);
}

/**
 * @brief 输出
 * @param out 输出源
 * @param level 缩进
*/
void SyntaxNode::write(std::ostream& out, int level, char end) {
	tab(out, level);
	out << "{" << std::endl;

	symbol->write(out, level + 1);

	tab(out, level + 1);
	out << "\"child\": [";

	int size = child.size();
	out << (size > 0? "" : "]") << std::endl;
	for (int i = 0; i < size; i++) {
		SyntaxNode* node = child[i];
		
		if (i < size - 1) {
			node->write(out, level + 2, ',');
		}
		else {
			node->write(out, level + 2, ']');
		}
	}

	tab(out, level);
	out << "}" << end << std::endl;
}

/**
 * @brief 获取该结点的所有孩子
 * @return 所有孩子
*/
NodeList* SyntaxNode::getChildren() {
	return &child;
}

/**
 * @brief 插入新的结点
 * @param symbol 符号
*/
void SyntaxTree::insert(Symbol* symbol) {
	SyntaxNode* node = new SyntaxNode(symbol);

	constructStack.push_back(node);
}

/**
 * @brief 插入新结点，并将构造栈中的若干结点作为新结点的孩子
 * @param symbol 符号
 * @param size 子结点数量
*/
void SyntaxTree::construct(Symbol* symbol, int size) {
	int len = constructStack.size();
	SyntaxNode* node = new SyntaxNode(symbol);

	for (int i = len - size; i < len; i++) {
		node->addChild(constructStack[i]);
	}

	while (size--) {
		constructStack.pop_back();
	}

	constructStack.push_back(node);

	if (constructStack.size() == 1) {
		head = node;
	}
}

SyntaxNode* SyntaxTree::getHead() {
	return head;
}

void deleteNode(SyntaxNode* node) {
	if (node == NULL)
		return;

	for (SyntaxNode* n : *node->getChildren()) {
		deleteNode(n);
	}
	delete node;
}

SyntaxTree:: ~SyntaxTree() {
	deleteNode(head);
}