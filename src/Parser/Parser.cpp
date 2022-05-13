#include "Parser/Parser.h"
#include "Lexer/Lexer.h"
#include "Output/Output.h"
#include "Output/Log.h"
#include "Output/Exception.h"
#include "Optimization/Optimization.h"
#include <iostream>
#include <iomanip>

extern void tab(Log* log, int level);
extern int getWidth(int number, std::string str);
extern std::string convertToString(Token& token);

/**
 * @brief LR(1)�﷨���������캯��
 * @param fileName �ķ��ļ���
*/
Parser::Parser(const char* fileName) : machine(fileName), analyzer() {
	Output* output = Output::getInstance();
	output->sendMessage("start build machine...");
	machine.create();
	output->sendMessage("start build SemanticAction...");
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
*/
void Parser::analysis(Lexical::Lexer* lexer) {
	// ��ȡToken��
	lexer->run();

	Output& output = *Output::getInstance();
	std::vector<Token>* tokens = lexer->getTokens();
	int len = tokens->size();
	Grammer* grammer = machine.getGrammer();

	stateStack.push_back(0);
	symbolStack.push_back(grammer->getSymbol(String("#")));

	for (int i = 0; i < len; i++) {
		int nowState = stateStack[stateStack.size() - 1];
		Token token = (*tokens)[i];
		Terminator terminator(token);

		// ������������Ϣ���
		output.beginningMessage(&terminator, this);

		// �������ܴ�����﷨
		if (table[nowState].count(&terminator) == 0) {
			throw CompilerException("[ERROR] Syntax error!", token);
		}

		// ��Ҫ��ȡ�Ķ���
		Action action = table[nowState][&terminator];
		
		if (action.accept) {
			// �﷨��������������ɹ�
			output.sendMessage("accept!");
			analyzer.outputIntermediateCode();
			return;
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
			try {
				analyzer.reduce(product.symbolPoint, grammer->getProductionCount(product.symbolPoint) + product.productionIndex, pop_cnt);
			}
			catch (const CompilerException& e) {
				// ����������ִ��󣬼��������׳������쳣
				for (int i = 0; i < pop_cnt; i++) {
					if (product_str[i]->isEnd()) {
						Terminator* terminator = static_cast<Terminator*>(product_str[i]);
						throw CompilerException(e.getError(), terminator->getToken());
					}
				}
				throw;
			}
			
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
}

/*
* @brief ���м��������Ż�
*/
void Parser::optimize() {
	optimization.splitBlocks(analyzer.getIntermediateCode());
	optimization.optimize();
}

/*
* @brief ����Ŀ�����
*/
void Parser::generate() {
	generator.init(analyzer.getGlobalSize());
	vector<Block>& blocks = optimization.getBlocks();
	for (Block& block : blocks) {
		generator.generateBatch(block.getInnerCode(), optimization.getFunctionEntry());
	}

	generator.out();
}

/**
 * @brief ������
*/
void Parser::writeTable() {
	using namespace std;
	vector<State>* states = machine.getStates();
	int size = states->size();			// ״̬����
	Log* log = FileLog::getInstance("table.txt");
	ostream& out = log->origin();

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