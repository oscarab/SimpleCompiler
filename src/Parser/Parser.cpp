#include "Parser/Parser.h"
#include "Lexer/Lexer.h"
#include "Output/Output.h"
#include "Optimization/Optimization.h"
#include <iostream>
#include <iomanip>

extern void tab(std::ostream& out, int level);
extern int getWidth(int number, std::string str);
extern std::string convertToString(Token& token);
extern Output output;

/**
 * @brief LR(1)�﷨���������캯��
 * @param fileName �ķ��ļ���
*/
Parser::Parser(const char* fileName) : machine(fileName), analyzer() {
	machine.create();

	analyzer.generateSemanticAction(machine.getGrammer(), "Semantic.txt");
	analyzer.distributeAttributeSymbols("Classify.txt");
}

/**
 * @brief ����LR(1)������
*/
void Parser::createTable() {
	std::vector<State>* states = machine.getStates();
	Transfer* transfer = machine.getTransfer();
	Grammer* grammer = machine.getGrammer();
	int ssize = transfer->size();

	// ��������״̬
	for (int i = 0; i < ssize; i++) {
		SymMapInt& stateTrans = (*transfer)[i];
		std::unordered_map<Symbol*, Action> tuple;
		State& state = (*states)[i];

		// ����ÿһ����Ŀ
		for (auto iter = state.begin(); iter != state.end(); iter++) {
			Item item = *iter;
			Symbol* sym = NULL;
			Action action;
			
			if (item.isAccept()) {
				// ����
				action.accept = true;
				tuple[grammer->getSymbol("#")] = action;
			}
			else if (sym = item.isMoveIn()) {
				// ����
				action.action = sym->isEnd();
				action.go = stateTrans[sym];
				tuple[sym] = action;
			}
			else if (item.isReduction()) {
				// ��Լ
				action.action = true;
				action.reduction = true;
				action.product = item.getProduction();
				tuple[item.getForward()] = action;
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
	if (!lexer->run(output[1])) return false;

	std::vector<Token>* tokens = lexer->getTokens();
	int len = tokens->size();
	Grammer* grammer = machine.getGrammer();

	stateStack.push_back(0);
	symbolStack.push_back(grammer->getSymbol(String("#")));
	analyzer.setOutStream(output[1]);

	for (int i = 0; i < len; i++) {
		int nowState = stateStack[stateStack.size() - 1];
		Token token = (*tokens)[i];
		Terminator terminator(token);

		// ������������Ϣ���
		output.beginningMessage(&terminator, this);

		// �������ܴ�����﷨
		if (table[nowState].count(&terminator) == 0) {
			output.sendMessage("syntax error!");
			return false;
		}

		// ��Ҫ��ȡ�Ķ���
		Action action = table[nowState][&terminator];
		
		if (action.accept) {
			output.sendMessage("accept!");
			analyzer.outputIntermediateCode(output[2]);
			return true;
		}
		else if (action.reduction) {
			// ��Լ
			Production product = action.product;
			SymbolChain& product_str = (*product.symbolPoint->getProductions())[product.productionIndex];
			
			int pop_cnt = product_str.size();
			if (product_str[0]->isEnd() && static_cast<Terminator*>(product_str[0])->getToken().isEmpty()) {
				pop_cnt = 0;
			}

			int reduce_cnt = pop_cnt;
			analyzer.reduce(product.symbolPoint, grammer->getProductionCount(product.symbolPoint) + product.productionIndex, pop_cnt);
			tree.construct(product.symbolPoint, pop_cnt);

			while (pop_cnt--) {
				symbolStack.pop_back();
				stateStack.pop_back();
			}

			nowState = stateStack.back();
			int dest = table[nowState][product.symbolPoint].go;

			symbolStack.push_back(product.symbolPoint);
			stateStack.push_back(dest);
			i--;

			// ��������Ϣ
			output.reductionMessage(reduce_cnt, dest);
		}
		else {
			// ����
			Symbol* read_sym = new Terminator(token);
			
			tree.insert(read_sym);
			symbolStack.push_back(read_sym);
			stateStack.push_back(action.go);
			analyzer.moveIn(read_sym);

			// ��������Ϣ
			output.movingMessage(action.go);
		}
	}
	return false;
}

bool Parser::optimize() {
	optimization.splitBlocks(analyzer.getIntermediateCode());
	optimization.optimize();
	return true;
}

/**
 * @brief ������
*/
void Parser::writeTable() {
	using namespace std;
	vector<State>* states = machine.getStates();
	int size = states->size();			// ״̬����

	// ɸѡ�ս���ͷ��ս��
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

	/* ������ */

	// �����п�
	int whole_w = 0;
	int term_cnt = terminator.size(), non_term_cnt = non_terminator.size();
	vector<int> width;
	width.push_back(getWidth(size, "״̬") + 4);
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

	// ��ͷ
	output[0] << "|";
	output[0] << setw(width[0]) << "״̬";
	output[0] << "|";
	for (int i = 0; i < term_cnt; i++) {
		Token token = terminator[i]->getToken();
		output[0] << setw(width[i + 1]) << convertToString(token);
		output[0] << "|";
	}
	for (int i = 0; i < non_term_cnt; i++) {
		output[0] << setw(width[term_cnt + i + 1]) << non_terminator[i]->getName();
		output[0] << "|";
	}
	output[0] << std::endl;

	// ����״̬
	for (int i = 0; i < size; i++) {
		output[0] << "|";
		output[0] << setw(width[0]) << i;
		output[0] << "|";
		for (int j = 0; j < term_cnt + non_term_cnt; j++) {
			Symbol* sym = j < term_cnt? (Symbol*) terminator[j] : non_terminator[j - term_cnt];
			auto iter = table[i].find(sym);
			output[0] << setw(width[j + 1]);

			if (iter != table[i].end()) {
				Action action = (*iter).second;
				output[0] << action.toString();
			}
			else {
				output[0] << " ";
			}
			output[0] << "|";
		}
		output[0] << std::endl;
	}
}

/**
 * @brief ջ ���ݵ���ʾ
 * @param out ���
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