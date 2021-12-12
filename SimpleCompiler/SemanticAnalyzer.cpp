#include "SemanticAnalyzer.h"
#include "AttributeSymbol.h"
#include "Grammer.h"
#include <fstream>
#include <sstream>
#include <iomanip>

Property packProperty(String& str) {
	int point = str.find(".");

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
	int start_pos = 0;
	int end_pos = 0;

	// 分割出每一个参数
	while (start_pos < str.size()) {
		end_pos = str.find(",", start_pos);
		end_pos = end_pos == str.npos ? str.size() : end_pos;

		String arg = str.substr(start_pos, end_pos - start_pos);
		action.addProperties(packProperty(arg));

		start_pos = end_pos + 1;
	}
}

/****************************符号表成员函数****************************/

IDTable::IDTable(IDTable* pre) {
	previous = pre;
	width = 0;
}

/**
 * @brief 往符号表中插入新符号
 * @param name 名字
 * @param type 类型
 * @param w 宽度
*/
void IDTable::insert(String name, String type, int w) {
	table[name] = ValTable{ type, width, false };
	width += w;
}

/**
 * @brief 往符号表中插入新过程标识符
 * @param name 名字
 * @param type 返回类型
*/
void IDTable::insertProc(String name, String type, bool isOuter) {
	if (isOuter) {
		table[name] = ValTable{ type, int(next.size() - 1), true };
	}
	else {
		table[name] = ValTable{ type, -1, true };
	}
}

/**
 * @brief 寻找符号
 * @param name 名字
 * @return -1若没找，若找到返回其相对地址
*/
int IDTable::find(String name) {
	auto p = table.find(name);
	if (p == table.end()) return -1;
	return (*p).second.offset;
}

/**
 * @brief 寻找函数过程
 * @param name 名字
 * @return -100若没找到，若找到返回其对应的next指针位置
*/
int IDTable::findProc(String name) {
	auto p = table.find(name);
	if (p == table.end()) return -1;
	if (!(*p).second.isFunc) return -1;
	return (*p).second.offset;
}

IDTable* IDTable::getPrevious() { 
	return previous; 
}

void IDTable::addNext(IDTable* next_table) {
	next.push_back(next_table);
}

/*********************************************************************/



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
		int start_pos = 0;
		int end_pos = 0;
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
			int stp = 0;
			int edp = str_action.find(";", stp);
			std::vector<SemanticAction> actions;

			// 分割出某一个语义动作
			while (edp != str_action.npos && stp < str_action.size()) {
				edp = str_action.find(";", stp);
				String str_subaction = str_action.substr(stp, edp - stp);

				int equal_pos = str_subaction.find("=");
				int func_pos = str_subaction.find("(");

				// 分为三种情况，单一调用函数、单一赋值、二者结合
				if (equal_pos != str_subaction.npos && str_subaction.find(":=") == str_subaction.npos) {
					String left_valu = str_subaction.substr(0, equal_pos);
					String right = str_subaction.substr(equal_pos + 1);
					
					if (func_pos != str_subaction.npos) {
						SemanticAction action;
						action.setOperator(SemanticOperator::ALL);
						action.setFunctionExecutor(str_subaction.substr(equal_pos + 1, func_pos - equal_pos - 1));
						action.addProperties(packProperty(left_valu));
						if (func_pos + 1 < str_subaction.size() - 1) {
							packParameter(str_subaction.substr(func_pos + 1, str_subaction.size() - 2 - func_pos), action);
						}
						actions.push_back(action);
					}
					else {
						SemanticAction action;
						action.setOperator(SemanticOperator::ASSIGN);
						action.addProperties(packProperty(left_valu));
						action.addProperties(packProperty(right));
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
	// 逐行读入
	while (getline(fin, line)) {
		int pos = line.find(" ");
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
		else if (class_name == "ControlSymbol") {
			non_terminator = new ControlSymbol(non_terminator->getName());
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

void SemanticAnalyzer::emite(String op, String arg1, String arg2, String res) {
	intermediateCode.push_back(Quaternion(op, arg1, arg2, res));
}

int SemanticAnalyzer::nextstat(int next) {
	return intermediateCode.size() + next - 1;
}

void SemanticAnalyzer::backpatch(int index, String addr) {
	intermediateCode[index].setResult(addr);
}

String SemanticAnalyzer::newtemp() {
	return String("T") + std::to_string(newTempCount++);
}

String SemanticAnalyzer::lookup(String name) {
	IDTable* p = nowTable;
	
	while (p) {
		int place = p->find(name);
		if (place != -1) {
			std::stringstream sstream;
			sstream << "0x" << std::hex << place;
			return sstream.str();
		}
		p = p->getPrevious();
	}

	(*out) << "[ERROR] undeclared identifier: " << name;
	exit(0);
}

void SemanticAnalyzer::lookupproc(String name) {
	IDTable* p = nowTable;

	while (p) {
		int place = p->findProc(name);
		if (place != -1) {
			return;
		}
		p = p->getPrevious();
	}

	(*out) << "[ERROR] undeclared identifier: " << name;
	exit(0);
}

void SemanticAnalyzer::notlookup(String name) {
	IDTable* p = nowTable;

	int place = p->find(name);
	if (place != -1) {
		(*out) << "[ERROR] identifier redefinition: " << name;
		exit(0);
	}
}
void SemanticAnalyzer::enter(String name, String type, int width) {
	nowTable->insert(name, type, width);
}

void SemanticAnalyzer::enterproc(String name, String type, String arg) {
	nowTable->insertProc(name, type, arg == "1");
}

void SemanticAnalyzer::mktable() {
	IDTable* newTable = new IDTable(nowTable);
	if (nowTable != NULL) {
		nowTable->addNext(newTable);
	}
	nowTable = newTable;
}

void SemanticAnalyzer::bktable() {
	nowTable = nowTable->getPrevious();
}

/*********************************************************************/


/******************************四元式成员函数**************************/

Quaternion::Quaternion(String _op, String _arg1, String _arg2, String _result) {
	operate = _op;
	arg1 = _arg1;
	arg2 = _arg2;
	result = _result;
}

void Quaternion::setResult(String result) {
	this->result = result;
}

void Quaternion::output(std::ostream& out) {
	out << std::setw(8) << operate << " " << std::setw(8) << arg1 << " " << std::setw(8) << arg2 << " " << std::setw(8) << result << std::endl;
}

/*********************************************************************/