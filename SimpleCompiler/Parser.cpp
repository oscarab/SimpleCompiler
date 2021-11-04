#include "Parser.h"
#include "Lexer.h"

/**
 * @brief LR(1)�﷨���������캯��
 * @param fileName �ķ��ļ���
*/
Parser::Parser(const char* fileName) : machine(fileName) {
	machine.create();
}

/**
 * @brief ����LR(1)������
*/
void Parser::createTable() {
	std::vector<State>* states = machine.getStates();
	Transfer* transfer = machine.getTransfer();
	int ssize = transfer->size();

	// ��������״̬
	for (int i = 0; i < ssize; i++) {
		SymMapInt& stateTrans = (*transfer)[i];
		std::unordered_map<Symbol, Action> tuple;
		State& state = (*states)[i];

		// ����ÿһ����Ŀ
		for (auto iter = state.begin(); iter != state.end(); iter++) {
			Item item = *iter;
			Symbol* sym = NULL;
			Action action;
			
			if (item.isAccept()) {
				// ����
				action.accept = true;
				tuple[Symbol(Token(TokenType::END, TokenAttribute::None))] = action;
			}
			else if (sym = item.isMoveIn()) {
				// ����
				action.action = true;
				action.go = stateTrans[*sym];
				tuple[*sym] = action;
			}
			else if (item.isReduction()) {
				// ��Լ
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
 * @brief ʹ��LR(1)�����﷨����
 * @param fileName ��Ҫ�����Ĵ����ļ�
*/
void Parser::analysis(const char* fileName) {
	Lexical::Lexer lexer(fileName);
	lexer.run();

	// ��ȡToken��
	std::vector<Token>* tokens = lexer.getTokens();
	int len = tokens->size();
	Grammer* grammer = machine.getGrammer();
	stateStack.push_back(0);

	for (int i = 0; i < len; i++) {
		int nowState = stateStack[stateStack.size() - 1];
		Token token = (*tokens)[i];
		token.setDefaultIndex();
		Symbol symbol(token);

		// �������ܴ�����﷨
		if (table[nowState].count(symbol) == 0) {
			break;
		}

		// ��Ҫ��ȡ�Ķ���
		Action action = table[nowState][symbol];
		
		if (action.accept) {
			int a = 1;
		}
		else if (action.reduction) {
			// ��Լ
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
			// ����
			Symbol* read_sym = grammer->getSymbol(&symbol);
			
			tree.insert(read_sym);
			symbolStack.push_back(read_sym);
			stateStack.push_back(action.go);
		}
	}
}

/**
 * @brief �﷨����㹹��
 * @param sym ����
*/
SyntaxNode::SyntaxNode(Symbol* sym) {
	symbol = sym;
}

/**
 * @brief ��������Ӻ���
 * @param node ���ӽ��
*/
void SyntaxNode::addChild(SyntaxNode* node) {
	child.push_back(node);
}

/**
 * @brief ��ȡ�ý������к���
 * @return ���к���
*/
NodeList* SyntaxNode::getChildren() {
	return &child;
}

/**
 * @brief �����µĽ��
 * @param symbol ����
*/
void SyntaxTree::insert(Symbol* symbol) {
	SyntaxNode* node = new SyntaxNode(symbol);

	constructStack.push_back(node);
}

/**
 * @brief �����½�㣬��������ջ�е����ɽ����Ϊ�½��ĺ���
 * @param symbol ����
 * @param size �ӽ������
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