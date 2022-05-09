#include "Semantic/SemanticAnalyzer.h"
#include "Semantic/AttributeSymbol.h"
#include "Parser/Grammer.h"
#include <fstream>
#include <sstream>
#include <iomanip>

Property packProperty(String& str) {
	size_t point = str.find(".");

	if (point == str.npos) {
		if (str.find("'") == str.npos) {
			return Property{ -1, str };
		}
		return Property{ 0, str.substr(1, str.size() - 2) };
	}
	else {
		int index = std::stoi(str.substr(0, point));
		return Property{ index, str.substr(point + 1) };
	}
}

void packParameter(String str, SemanticAction& action) {
	size_t start_pos = 0, end_pos = 0;

	// 分割出每一个参数
	while (start_pos < str.size()) {
		end_pos = str.find(",", start_pos);
		end_pos = end_pos == str.npos ? str.size() : end_pos;

		String arg = str.substr(start_pos, end_pos - start_pos);
		action.addProperty(packProperty(arg));

		start_pos = end_pos + 1;
	}
}


/****************************语义分析器成员函数*************************/

/**
 * @brief 根据语义规则的文件产生对应的语义动作
 * @param grammer 现有的文法
 * @param file_name 语义规则的文件
*/
void SemanticAnalyzer::generateSemanticAction(Grammer* grammer, const char* file_name) {
	// 读取并分析语义规则文件 Semantic.txt
	std::ifstream semanticFile(file_name);
	if (!semanticFile.is_open()) {
		std::cout << "fail to open semantic file!" << std::endl;
		exit(0);
	}

	String line;
	// 逐行读入
	while (getline(semanticFile, line)) {
		size_t start_pos = 0, end_pos = 0;
		if (line == "$$") {
			semanticActions.push_back(std::vector<SemanticAction>());
			continue;
		}

		// 切割出每一个语义动作块
		while (start_pos < line.size()) {
			end_pos = line.find("|", start_pos);
			end_pos = end_pos == line.npos ? line.size() : end_pos;

			String str_action = line.substr(start_pos, end_pos - start_pos);
			if (str_action == "$$") {
				semanticActions.push_back(std::vector<SemanticAction>());
				start_pos = end_pos + 1;
				continue;
			}
			size_t stp = 0, edp = str_action.find(";", stp);
			std::vector<SemanticAction> actions;

			// 分割出某一个语义动作
			while (edp != str_action.npos && stp < str_action.size()) {
				edp = str_action.find(";", stp);
				String str_subaction = str_action.substr(stp, edp - stp);

				size_t equal_pos = str_subaction.find("=");
				size_t func_pos = str_subaction.find("(");

				// 分为三种情况，单一调用函数、单一赋值、二者结合
				if (equal_pos != str_subaction.npos && str_subaction.find(":=") == str_subaction.npos) {
					String left_valu = str_subaction.substr(0, equal_pos);
					String right = str_subaction.substr(equal_pos + 1);
					
					if (func_pos != str_subaction.npos) {
						SemanticAction action;
						action.setOperator(SemanticOperator::ALL);
						action.setFunctionExecutor(str_subaction.substr(equal_pos + 1, func_pos - equal_pos - 1));
						action.addProperty(packProperty(left_valu));
						if (func_pos + 1 < str_subaction.size() - 1) {
							packParameter(str_subaction.substr(func_pos + 1, str_subaction.size() - 2 - func_pos), action);
						}
						actions.push_back(action);
					}
					else {
						SemanticAction action;
						action.setOperator(SemanticOperator::ASSIGN);
						action.addProperty(packProperty(left_valu));
						action.addProperty(packProperty(right));
						actions.push_back(action);
					}
				}
				else {
					SemanticAction action;
					action.setOperator(SemanticOperator::FUNCTION);
					action.setFunctionExecutor(str_subaction.substr(0, func_pos));
					if (func_pos + 1 < str_subaction.size() - 1) {
						packParameter(str_subaction.substr(func_pos + 1, str_subaction.size() - 2 - func_pos), action);
					}
					actions.push_back(action);
				}
				stp = edp + 1;
			}
			
			semanticActions.push_back(actions);
			start_pos = end_pos + 1;
		}
	}
}

/**
 * @brief 分析每一个文法符号应该要对应的属性符号
 * @param file_name 
*/
void SemanticAnalyzer::distributeAttributeSymbols(const char* file_name) {
	// 读取并分析符号属性文件 AttributeSymbol.txt
	std::ifstream fin(file_name);
	if (!fin.is_open()) {
		std::cout << "fail to open attribute file!" << std::endl;
		exit(0);
	}

	String line;
	size_t pos;
	// 逐行读入
	while (getline(fin, line)) {
		pos = line.find(" ");
		symbolDistribute[line.substr(0, pos)] = line.substr(pos + 1);
	}
}

/**
 * @brief 构造带有属性的文法符号
 * @param symbol 符号
 * @return 带属性的文法符号
*/
Symbol* SemanticAnalyzer::createAttributeSymbol(Symbol* symbol) {
	if (symbol->isEnd()) {
		Terminator* terminator = static_cast<Terminator*>(symbol);
		terminator = new LeafSymbol(terminator->getToken());
		terminator->setupReflect();
		return terminator;
	}
	else {
		NonTerminator* non_terminator = static_cast<NonTerminator*>(symbol);
		String class_name = symbolDistribute[non_terminator->getName()];

		if (class_name == "VariableSymbol") {
			non_terminator = new VariableSymbol(non_terminator->getName());
			non_terminator->setupReflect();
			return non_terminator;
		}
		else if (class_name == "OperatorSymbol") {
			non_terminator = new OperatorSymbol(non_terminator->getName());
			non_terminator->setupReflect();
			return non_terminator;
		}
		else if (class_name == "EpsilonSymbol") {
			non_terminator = new EpsilonSymbol(non_terminator->getName());
			non_terminator->setupReflect();
			return non_terminator;
		}
	}
}

void SemanticAnalyzer::setOutStream(std::ostream& _out) {
	out = &_out;
}

/**
 * @brief 按偏移量从符号栈中取文法符号
 * @param index 偏移量
 * @return 文法符号
*/
Symbol* SemanticAnalyzer::getSymbolFromStack(int index) {
	if (index > reductionCount) {
		return reductionResult;
	}
	return stateStack[stateStack.size() - index];
}

/**
 * @brief 输出中间代码
*/
void SemanticAnalyzer::outputIntermediateCode(std::ostream& out) {
	int p = 0;
	for (Quaternion& q : intermediateCode) {
		out << std::setw(5) << p;
		p++;
		q.output(out);
	}
}

void SemanticAnalyzer::moveIn(Symbol* symbol) {
	stateStack.push_back(createAttributeSymbol(symbol));
}

void SemanticAnalyzer::reduce(Symbol* symbol, int index, int count) {
	reductionCount = count;
	reductionResult = createAttributeSymbol(symbol);

	// 执行这个符号对应的语义规则
	std::vector<SemanticAction>& actions = semanticActions[index];
	for (SemanticAction& action : actions) {
		action.execute(this);
	}

	while (count--) {
		delete stateStack.back();
		stateStack.pop_back();
	}
	stateStack.push_back(reductionResult);
}

std::vector<Quaternion>& SemanticAnalyzer::getIntermediateCode() {
	return intermediateCode;
}

unsigned int SemanticAnalyzer::getGlobalSize() {
	return nowTable->getWidth();
}

void SemanticAnalyzer::emite(String op, String arg1, String arg2, String res) {
	Variable var1, var2, target;
	if (op == "jal") {
		res = std::to_string(nowTable->findProcessPosition(res));
	}
	var1.name = arg1;
	var2.name = arg2;
	target.name = res;
	var1.address = nowTable->find(var1.name, true);
	var2.address = nowTable->find(var2.name, true);
	target.address = nowTable->find(target.name, true);

	intermediateCode.push_back(Quaternion(op, var1, var2, target));
}

int SemanticAnalyzer::nextstat(int next) {
	return intermediateCode.size() + next - 1;
}

void SemanticAnalyzer::backpatch(int index, String addr) {
	intermediateCode[index].setResult(Variable{ addr, 0 });
}

String SemanticAnalyzer::newtemp() {
	String temp = String("#T") + std::to_string(newTempCount++);
	nowTable->insert(temp, "int", 4);
	return temp;
}

String SemanticAnalyzer::lookup(String name) {
	int place = nowTable->find(name, true);
	if (place != -1) {
		return name;
	}

	std::cout << "[ERROR] undeclared identifier: " << name;
	(*out) << "[ERROR] undeclared identifier: " << name;
	exit(0);
}

void SemanticAnalyzer::lookupproc(String name, String type) {
	IDTable* p = nowTable;

	while (p) {
		int place = p->findProcess(name);
		if (place != -100) {

			bool type_check = false;
			if (place == -1) {
				type_check = p->checkParameters(type);
			}
			else {
				type_check = p->getNext(place)->checkParameters(type);
			}
			if (!type_check) {
				std::cout << "[ERROR] parameter type mismatch: " << name;
				(*out) << "[ERROR] parameter type mismatch: " << name;
				exit(0);
			}
				
			return;
		}
		p = p->getPrevious();
	}

	std::cout << "[ERROR] undeclared identifier: " << name;
	(*out) << "[ERROR] undeclared identifier: " << name;
	exit(0);
}

void SemanticAnalyzer::checkmain() {
	int ind = nowTable->findProcess("main");
	if (ind == -100) {
		std::cout << "[ERROR] no entry defined";
		(*out) << "[ERROR] no entry defined";
		exit(0);
	}
}

void SemanticAnalyzer::notlookup(String name) {
	IDTable* p = nowTable;

	int place = p->find(name, false);
	if (place != -1) {
		std::cout << "[ERROR] identifier redefinition: " << name;
		(*out) << "[ERROR] identifier redefinition: " << name;
		exit(0);
	}
}

void SemanticAnalyzer::enter(String name, String type, int width) {
	nowTable->insert(name, type, width);
}

void SemanticAnalyzer::enterproc(String name, String type, String pos, String arg) {
	nowTable->insertProcess(name, type, std::stoi(pos), arg == "1");
	if (arg == "0" && name == "main") {
		backpatch(0, std::to_string(nextstat(1)));
	}
}

void SemanticAnalyzer::mktable() {
	IDTable* newTable = nullptr;
	if (nowTable != NULL) {
		int offset = nowTable->getPrevious() != NULL ? nowTable->getWidth() : 0;
		newTable = new IDTable(nowTable, offset);
		nowTable->addNext(newTable);
	}
	else {
		newTable = new IDTable(nowTable, 0);
	}
	nowTable = newTable;
}

void SemanticAnalyzer::bktable() {
	if (nowTable->getPrevious() != NULL) {
		nowTable = nowTable->getPrevious();
	}
}

String SemanticAnalyzer::lookuptype(String name) {
	IDTable* p = nowTable;

	while (p) {
		String type = p->getType(name);
		if (type != "") {
			return type;
		}
		p = p->getPrevious();
	}

	std::cout << "[ERROR] undeclared identifier: " << name;
	(*out) << "[ERROR] undeclared identifier: " << name;
	exit(0);
}

void SemanticAnalyzer::checktype(String type1, String type2) {
	if (type1 == "void" || type2 == "void" || type1 != type2) {
		std::cout << "[ERROR] error type: " << type1 << " and " << type2;
		(*out) << "[ERROR] error type: " << type1 << " and " << type2;
		exit(0);
	}
}

void SemanticAnalyzer::addpara(String type) {
	nowTable->addParameter(type);
}

void SemanticAnalyzer::enterarray(String name, String type, int width, int dim) {
	nowTable->insertArray(name, type, width, dim);
	emite("dec", "_", "_", std::to_string(width));
}

SemanticAnalyzer::~SemanticAnalyzer() {
	delete nowTable;
}

/*********************************************************************/


/******************************四元式成员函数**************************/

Quaternion::Quaternion(String op, Variable arg1, Variable arg2, Variable tartget) {
	operate = op;
	parameter1 = arg1;
	parameter2 = arg2;
	result = tartget;
	label = "";
}

void Quaternion::setResult(Variable result) {
	this->result = result;
}

void Quaternion::setLabel(String label) {
	this->label = label;
}

std::string Quaternion::getLabel() {
	return label;
}

bool Quaternion::isJump() {
	return (operate == ">" || operate == "<" || operate == "==" || operate == "!="
		|| operate == ">=" || operate == "<=" || operate == "j"
		|| operate == "jal");
}

void Quaternion::output(std::ostream& out) {
	out << std::setw(8) << operate << " " << std::setw(8) << parameter1.name << " " << std::setw(8) << parameter2.name << " " << std::setw(8) << result.name << std::endl;
}

/*********************************************************************/