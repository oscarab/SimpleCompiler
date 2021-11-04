#include "Machine.h"
#include <unordered_set>
#include <algorithm>

/**
 * @brief 产生式的构造函数
 * @param symbol 左部的非终结符
 * @param symInd 非终结符的下标位置
 * @param product 非终结符所拥有的产生式的位置
*/
Production::Production(Symbol* symbol, int symInd, int product) {
	symbolPoint = symbol;
	symbolIndex = symInd;
	productionIndex = product;
}

/**
 * @brief 项目的构造函数
 * @param prod 产生式
 * @param p 点 的位置
 * @param forwd 展望
*/
Item::Item(Production prod, int p, Symbol* forwd) : production(prod) {
	point = p;
	forward = forwd;
}

/**
 * @brief 点 位置后移一位
*/
void Item::movePoint() {
	point++;
}

/**
 * @brief 判断是否为归约项目
 * @return true 若是归约项目
*/
bool Item::isReduction() const {
	PSymbol& prod = (*production.symbolPoint->getProductions())[production.productionIndex];
	return point >= prod.size();
}

/**
 * @brief 判断是否为移进项目
 * @return Symbol* 要移入的符号
*/
Symbol* Item::isMoveIn() const {
	PSymbol& prod = (*production.symbolPoint->getProductions())[production.productionIndex];
	
	if (point < prod.size()) {
		return prod[point];
	}
	
	return NULL;
}

/**
 * @brief 判断是否到达接受
 * @return true 若接受
*/
bool Item::isAccept() const {
	PSymbol& prod = (*production.symbolPoint->getProductions())[production.productionIndex];
	return point >= prod.size() && production.symbolIndex == 0;
}

/**
 * @brief 获取产生式
 * @return Production 产生式
*/
Production Item::getProduction() const {
	return production;
}

/**
 * @brief 获取点 位置
 * @return int 位置
*/
int Item::getPoint() const {
	return point;
}

/**
 * @brief 获取展望符号
 * @return Symbol* 展望
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
 * @brief 自动机的构造函数
 * @param fileName 文法文件名
*/
Machine::Machine(const char* fileName) {
	grammer = new Grammer(fileName);
	grammer->solveCanEmpty();
	grammer->solveFirst();

	// 建立第0个状态，里面只有一个项目
	// S'->·S  #
	State i0;
	i0.insert(	Item(
					Production((*grammer->getSymbols())[0], 0, 0), 
					0, 
					grammer->getSymbol(String("#")))
			 );
	states.push_back(i0);
}

/**
 * @brief 计算closure集合
 * @param state 需要计算的项目集
*/
void Machine::solveClosure(State& state) {
	int size;
	do {
		size = state.size();
		for (auto p = state.begin(); p != state.end(); p++) {
			Item item = *p;
			Production prod_p = item.getProduction();

			// 获取产生式
			PSymbol* closure = &(*prod_p.symbolPoint->getProductions())[prod_p.productionIndex];

			if (item.getPoint() < closure->size()) {
				// 获取 点 后的第一个符号
				Symbol* symbol = (*closure)[item.getPoint()];

				// 当是非终结符时开始拓展
				if (!symbol->isEnd()) {

					// 获取这个非终结符的所有产生式
					std::vector<PSymbol>* all_prod = symbol->getProductions();
					int len_prod = all_prod->size();

					// 遍历所有产生式
					for (int j = 0; j < len_prod; j++) {
						// 求后续字符组成的串的FIRST集合
						PSymbol after_first;
						if (item.getPoint() < closure->size() - 1)
							after_first.insert(after_first.end(), closure->begin() + item.getPoint() + 1, closure->end());
						after_first.push_back(item.getForward());
						grammer->solveFirst(after_first);

						// 将求得的FIRST集里面的符号，都作为前瞻，组成一系列项目
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
 * @brief 创建自动机
*/
void Machine::create() {
	int pos = 0;
	transfer.push_back(SymMapInt());
	solveClosure(states[0]);

	while (pos < states.size()) {
		// 获取所有符号
		PSymbol* all_sym = grammer->getSymbols();

		// 用每一个符号去试探是否可以转移
		for (Symbol* sym : *all_sym) {
			State new_state;
			State& state = states[pos];

			for (auto p = state.begin(); p != state.end(); p++) {
				Item item = *p;
				Production prod_p = item.getProduction();
				PSymbol prod;

				// 获取产生式
				grammer->getProduction(prod_p.symbolIndex, prod_p.productionIndex, prod);

				// 遇到可转移的项目，加到新状态中
				if (item.getPoint() < prod.size() && *(prod[item.getPoint()]) == *sym) {
					item.movePoint();
					new_state.insert(item);
				}
			}

			// 产生出了新状态
			if (!new_state.empty()) {
				// 求新状态closure集合
				solveClosure(new_state);

				// 检查该状态是否已经存在
				auto p = std::find(states.begin(), states.end(), new_state);

				if (p != states.end()) {
					// 已经有该状态，直接连接
					transfer[pos][*sym] = p - states.begin();
				}
				else {
					// 创建新的状态
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
 * @brief 获取自动机内的所有状态
 * @return 所有状态
*/
std::vector<State>* Machine::getStates() {
	return &states;
}

/**
 * @brief 获取转移关系
 * @return 转移关系
*/
Transfer* Machine::getTransfer() {
	return &transfer;
}

/**
 * @brief 获取自动机依赖的文法
 * @return 文法
*/
Grammer* Machine::getGrammer() {
	return grammer;
}

/**
 * @brief 析构
*/
Machine::~Machine() {
	delete grammer;
}