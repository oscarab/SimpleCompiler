#include "Parser.h"
#include "Lexer.h"
#include <iostream>
#include <iomanip>

extern void tab(std::ostream& out, int level);
extern int getWidth(int number, std::string str);
extern std::string convertToString(Token& token);

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
				action.action = sym->isEnd();
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
 * @param lexer �ʷ�������
 * @return �Ƿ�����ɹ�
*/
bool Parser::analysis(Lexical::Lexer* lexer) {
	// ��ȡToken��
	std::vector<Token>* tokens = lexer->getTokens();
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
			return false;
		}

		// ��Ҫ��ȡ�Ķ���
		Action action = table[nowState][symbol];
		
		if (action.accept) {
			return true;
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
	return false;
}

/**
 * @brief ������
 * @param out ���Դ
*/
void Parser::writeTable(std::ostream& out) {
	using namespace std;
	vector<State>* states = machine.getStates();
	int size = states->size();			// ״̬����

	// ɸѡ�ս���ͷ��ս��
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

	/* ������ */

	// �����п�
	int whole_w = 0;
	int term_cnt = terminator.size(), non_term_cnt = non_terminator.size();
	vector<int> width;
	width.push_back(getWidth(size, "״̬") + 4);
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

	// ��ͷ
	out << "|";
	out << setw(width[0]) << "״̬";
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

	// ����״̬
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
 * @brief ��ȡ�﷨��
 * @return �﷨��
*/
SyntaxTree* Parser::getTree() {
	return &tree;
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
 * @brief ���
 * @param out ���Դ
 * @param level ����
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