#include "Parser.h"
#include "Lexer.h"

Parser::Parser(const char* fileName) : machine(fileName) {
	machine.create();
}

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

void Parser::analysis(const char* fileName) {
	Lexical::Lexer lexer("E:/编译原理/code.txt");
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
			Symbol* read_sym = (*grammer->getSymbols())[grammer->getSymbolIndex(&symbol)];
			
			symbolStack.push_back(read_sym);
			stateStack.push_back(action.go);
		}
	}
}