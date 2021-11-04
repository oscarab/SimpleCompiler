#include "Machine.h"
#include <unordered_set>
#include <algorithm>

/**
 * @brief ����ʽ�Ĺ��캯��
 * @param symbol �󲿵ķ��ս��
 * @param symInd ���ս�����±�λ��
 * @param product ���ս����ӵ�еĲ���ʽ��λ��
*/
Production::Production(Symbol* symbol, int symInd, int product) {
	symbolPoint = symbol;
	symbolIndex = symInd;
	productionIndex = product;
}

/**
 * @brief ��Ŀ�Ĺ��캯��
 * @param prod ����ʽ
 * @param p �� ��λ��
 * @param forwd չ��
*/
Item::Item(Production prod, int p, Symbol* forwd) : production(prod) {
	point = p;
	forward = forwd;
}

/**
 * @brief �� λ�ú���һλ
*/
void Item::movePoint() {
	point++;
}

/**
 * @brief �ж��Ƿ�Ϊ��Լ��Ŀ
 * @return true ���ǹ�Լ��Ŀ
*/
bool Item::isReduction() const {
	PSymbol& prod = (*production.symbolPoint->getProductions())[production.productionIndex];
	return point >= prod.size();
}

/**
 * @brief �ж��Ƿ�Ϊ�ƽ���Ŀ
 * @return Symbol* Ҫ����ķ���
*/
Symbol* Item::isMoveIn() const {
	PSymbol& prod = (*production.symbolPoint->getProductions())[production.productionIndex];
	
	if (point < prod.size()) {
		return prod[point];
	}
	
	return NULL;
}

/**
 * @brief �ж��Ƿ񵽴����
 * @return true ������
*/
bool Item::isAccept() const {
	PSymbol& prod = (*production.symbolPoint->getProductions())[production.productionIndex];
	return point >= prod.size() && production.symbolIndex == 0;
}

/**
 * @brief ��ȡ����ʽ
 * @return Production ����ʽ
*/
Production Item::getProduction() const {
	return production;
}

/**
 * @brief ��ȡ�� λ��
 * @return int λ��
*/
int Item::getPoint() const {
	return point;
}

/**
 * @brief ��ȡչ������
 * @return Symbol* չ��
*/
Symbol* Item::getForward() {
	return forward;
}

bool Item::operator<(const Item& item) const {
	if (point != item.point)
		return point < item.point;
	
	if (production.symbolIndex != item.production.symbolIndex)
		return production.symbolIndex < item.production.symbolIndex;

	if (production.productionIndex != item.production.productionIndex)
		return production.productionIndex < item.production.productionIndex;

	return item.forward < forward;
}

bool Item::operator==(const Item& item) const {
	return point == item.point && 
		production.symbolIndex == item.production.symbolIndex && 
		production.productionIndex == item.production.productionIndex &&
		item.forward == forward;
}

/**
 * @brief �Զ����Ĺ��캯��
 * @param fileName �ķ��ļ���
*/
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
					grammer->getSymbol(String("#")))
			 );
	states.push_back(i0);
}

/**
 * @brief ����closure����
 * @param state ��Ҫ�������Ŀ��
*/
void Machine::solveClosure(State& state) {
	int size;
	do {
		size = state.size();
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
						// ������ַ���ɵĴ���FIRST����
						PSymbol after_first;
						if (item.getPoint() < closure->size() - 1)
							after_first.insert(after_first.end(), closure->begin() + item.getPoint() + 1, closure->end());
						after_first.push_back(item.getForward());
						grammer->solveFirst(after_first);

						// ����õ�FIRST������ķ��ţ�����Ϊǰհ�����һϵ����Ŀ
						for (Symbol* sym_p : after_first) {
							Item new_item = Item(Production(symbol, grammer->getSymbolIndex(symbol), j),
								0, sym_p);
							state.insert(new_item);
						}

					}
				}
			}
		}
	} while (size != state.size());

}

/**
 * @brief �����Զ���
*/
void Machine::create() {
	int pos = 0;
	transfer.push_back(SymMapInt());
	solveClosure(states[0]);

	while (pos < states.size()) {
		// ��ȡ���з���
		PSymbol* all_sym = grammer->getSymbols();

		// ��ÿһ������ȥ��̽�Ƿ����ת��
		for (Symbol* sym : *all_sym) {
			State new_state;
			State& state = states[pos];

			for (auto p = state.begin(); p != state.end(); p++) {
				Item item = *p;
				Production prod_p = item.getProduction();
				PSymbol prod;

				// ��ȡ����ʽ
				grammer->getProduction(prod_p.symbolIndex, prod_p.productionIndex, prod);

				// ������ת�Ƶ���Ŀ���ӵ���״̬��
				if (item.getPoint() < prod.size() && *(prod[item.getPoint()]) == *sym) {
					item.movePoint();
					new_state.insert(item);
				}
			}

			// ����������״̬
			if (!new_state.empty()) {
				// ����״̬closure����
				solveClosure(new_state);

				// ����״̬�Ƿ��Ѿ�����
				auto p = std::find(states.begin(), states.end(), new_state);

				if (p != states.end()) {
					// �Ѿ��и�״̬��ֱ������
					transfer[pos][*sym] = p - states.begin();
				}
				else {
					// �����µ�״̬
					states.push_back(new_state);
					transfer.push_back(SymMapInt());
					transfer[pos][*sym] = states.size() - 1;
				}
			}
		}

		pos++;
	}
}

/**
 * @brief ��ȡ�Զ����ڵ�����״̬
 * @return ����״̬
*/
std::vector<State>* Machine::getStates() {
	return &states;
}

/**
 * @brief ��ȡת�ƹ�ϵ
 * @return ת�ƹ�ϵ
*/
Transfer* Machine::getTransfer() {
	return &transfer;
}

/**
 * @brief ��ȡ�Զ����������ķ�
 * @return �ķ�
*/
Grammer* Machine::getGrammer() {
	return grammer;
}

/**
 * @brief ����
*/
Machine::~Machine() {
	delete grammer;
}