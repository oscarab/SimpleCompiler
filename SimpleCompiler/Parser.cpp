#include "Parser.h"
#include "Lexer.h"

Parser::Parser(const char* fileName) : machine(fileName) {

}

void Parser::createTable() {
	std::vector<State>* states = machine.getStates();
	Transfer* transfer = machine.getTransfer();
	int ssize = transfer->size();

	// ��������״̬
	for (int i = 0; i < ssize; i++) {
		SymMapInt& stateTrans = (*transfer)[i];
		std::unordered_map<Symbol, Action> tuple;

		// ��������ת��
		for (auto iter = stateTrans.begin(); iter != stateTrans.end(); iter++) {
			Symbol sym = iter->first;
			int dest = iter->second;

			Action action;
			action.go = dest;
			action.accept = false;

			if (!sym.isEnd()) {
				// �������ս��������ת��
				action.action = false;
				action.reduction = false;
				tuple[sym] = action;
			}
			else {
				action.action = true;

				// �����ս��ʱ�жϽ��й�Լ�����ƽ������ǽ���
				State& state = (*states)[i];
				for (Item item : state) {
					if (item.isAccept(sym)) {
						action.accept = true;
						tuple[sym] = action;
					}
					else if (item.isMoveIn(sym)) {
						tuple[sym] = action;
					}
					else if (item.isReduction(sym)) {
						action.reduction = true;
						action.prod = item.getProduction();
						tuple[sym] = action;
					}
					else {
						continue;
					}
					break;
				}
			}
		}

		table.push_back(tuple);
	}
}

void Parser::analysis(const char* fileName) {
	Lexical::Lexer lexer("E:/����ԭ��/code.txt");
	lexer.run();

	// ��ȡToken��
	std::vector<Token>* tokens = lexer.getTokens();
	int len = tokens->size();
	Grammer* grammer = machine.getGrammer();

	for (int i = 0; i < len; i++) {
		int nowState = stateStack[stateStack.size() - 1];
		Token token = (*tokens)[i];
		Symbol symbol(token);
		// ��ȡ���뵥�ʵ�Symbol����
		Symbol* read_sym = (*grammer->getSymbols())[grammer->getSymbolIndex(&symbol)];

		// ����������﷨
		if (table[nowState].count(symbol) == 0) {
			break;
		}

		// ��Ҫ��ȡ�Ķ���
		Action action = table[nowState][symbol];
		
		if (action.accept) {

		}
		else if (action.reduction) {
			// ��Լ
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
		}
		else {
			// ����
			symbolStack.push_back(read_sym);
			stateStack.push_back(action.go);
		}
	}
}