#include "Parser.h"
#include "Lexer.h"

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
				action.action = true;
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
 * @param fileName 需要分析的代码文件
*/
void Parser::analysis(const char* fileName) {
	Lexical::Lexer lexer(fileName);
	lexer.run();

	// 获取Token流
	std::vector<Token>* tokens = lexer.getTokens();
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
			break;
		}

		// 需要采取的动作
		Action action = table[nowState][symbol];
		
		if (action.accept) {
			int a = 1;
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

void deleteNode(SyntaxNode* node) {
	for (SyntaxNode* n : *node->getChildren()) {
		deleteNode(n);
	}
	delete node;
}

SyntaxTree:: ~SyntaxTree() {
	deleteNode(head);
}