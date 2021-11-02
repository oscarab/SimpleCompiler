#include "Machine.h"
#include <unordered_set>
#include <algorithm>

Production::Production(Symbol* symbol, int symInd, int product) {
	symbolPoint = symbol;
	symbolIndex = symInd;
	productionIndex = product;
}

Item::Item(Production prod, int p, Token forwd) : production(prod), forward(forwd) {
	point = p;
}

void Item::movePoint() {
	point++;
}

bool Item::isReduction(Symbol& symbol) {
	PSymbol& prod = (*production.symbolPoint->getProductions())[production.symbolIndex];
	return point >= prod.size() && forward == symbol;
}

bool Item::isMoveIn(Symbol& symbol) {
	PSymbol& prod = (*production.symbolPoint->getProductions())[production.symbolIndex];
	return point < prod.size() && *prod[point] == symbol;
}

bool Item::isAccept(Symbol& symbol) {
	PSymbol& prod = (*production.symbolPoint->getProductions())[production.symbolIndex];
	return point >= prod.size() && production.symbolIndex == 0;
}

Production Item::getProduction() {
	return production;
}

int Item::getPoint() {
	return point;
}

Symbol* Item::getForward() {
	return &forward;
}

bool Item::operator<(const Item& item) const {
	if (point != item.point)
		return point < item.point;
	
	if (production.symbolIndex != item.production.symbolIndex)
		return production.symbolIndex < item.production.symbolIndex;

	if (production.productionIndex != item.production.productionIndex)
		return production.productionIndex < item.production.productionIndex;

	return item.forward == forward;
}

bool Item::operator==(const Item& item) const {
	return point == item.point && 
		production.symbolIndex == item.production.symbolIndex && 
		production.productionIndex == item.production.productionIndex &&
		item.forward == forward;
}

Machine::Machine(const char* fileName) {
	grammer = new Grammer(fileName);
	grammer->solveCanEmpty();
	grammer->solveFirst();

	// ������0��״̬������ֻ��һ����Ŀ
	// S'->��S  #
	State i0;
	i0.insert(	Item(
					Production((*grammer->getSymbols())[0], 0, 0), 
					0, 
					Token(TokenType::END, TokenAttribute::None))
			 );
	states.push_back(i0);
}

void Machine::solveClosure(int index) {
	State& state = states[index];
	int len = state.size();

	for (auto p = state.begin(); p != state.end(); p++) {
		Item item = *p;
		Production prod_p = item.getProduction();

		// ��ȡ����ʽ
		PSymbol* closure = &(*prod_p.symbolPoint->getProductions())[prod_p.productionIndex];

		if (item.getPoint() < closure->size()) {
			// ��ȡ �� ��ĵ�һ������
			Symbol* symbol = (*closure)[item.getPoint()];

			// ���Ƿ��ս��ʱ��ʼ��չ
			if (!symbol->isEnd()) {

				// ��ȡ������ս�������в���ʽ
				std::vector<PSymbol>* all_prod = symbol->getProductions();
				int len_prod = all_prod->size();

				// �������в���ʽ
				for (int j = 0; j < len_prod; j++) {
					// ������ַ���ɵĴ� ��FIRST����
					PSymbol after;
					if (item.getPoint() < closure->size() - 1)
						after.insert(after.end(), closure->begin() + item.getPoint() + 1, closure->end());
					after.push_back(item.getForward());
					grammer->solveFirst(after);

					// ����õ�FIRST������ķ��ţ�����Ϊǰհ�����һϵ����Ŀ
					for (Symbol* sym_p : after) {
						Item new_item = Item(Production(symbol, grammer->getSymbolIndex(symbol), j),
											0, sym_p->getToken());
						state.insert(new_item);
					}

				}
			}
		}
		
		// ������δ��������˵��������
		if (len != state.size()) {
			len = state.size();
		}
		else {
			break;
		}
		
	}

}

void Machine::create() {
	int pos = 0;
	transfer.push_back(SymMapInt());

	while (pos >= states.size()) {
		// ��״̬closure����
		solveClosure(pos);

		// ��pos��״̬
		State& state = states[pos];
		PSymbol* all_sym = grammer->getSymbols();

		// ��ÿһ������ȥ��̽�Ƿ����ת��
		for (Symbol* sym : *all_sym) {
			State new_state;

			for (auto p = state.begin(); p != state.end(); p++) {
				Item item = *p;
				Production prod_p = item.getProduction();
				PSymbol prod;

				// ��ȡ����ʽ
				grammer->getProduction(prod_p.symbolIndex, prod_p.productionIndex, prod);
				
				// ������ת�Ƶ���Ŀ���ӵ���״̬��
				if (item.getPoint() < prod.size() && *prod[item.getPoint()] == *sym) {
					item.movePoint();
					new_state.insert(item);
				}
			}

			// ����������״̬
			if (!new_state.empty()) {
				auto p = std::find(states.begin(), states.end(), new_state);

				if (p != states.end()) {
					transfer[pos][*sym] = p - states.begin();
				}
				else {
					states.push_back(new_state);
					transfer.push_back(SymMapInt());
					transfer[pos][*sym] = states.size() - 1;
				}
			}
		}

		pos++;
	}
}

std::vector<State>* Machine::getStates() {
	return &states;
}

Transfer* Machine::getTransfer() {
	return &transfer;
}

Grammer* Machine::getGrammer() {
	return grammer;
}

Machine::~Machine() {
	delete grammer;
}