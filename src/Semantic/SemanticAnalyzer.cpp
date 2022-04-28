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

	// �ָ��ÿһ������
	while (start_pos < str.size()) {
		end_pos = str.find(",", start_pos);
		end_pos = end_pos == str.npos ? str.size() : end_pos;

		String arg = str.substr(start_pos, end_pos - start_pos);
		action.addProperty(packProperty(arg));

		start_pos = end_pos + 1;
	}
}

/****************************���ű��Ա����****************************/

IDTable::IDTable(IDTable* prev) {
	previous = prev;
	width = 0;
}

/**
 * @brief �����ű��в����·���
 * @param name ����
 * @param type ����
 * @param w ���
*/
void IDTable::insert(String name, String type, int w) {
	table[name] = ValTable{ type, width, false };
	width += w;
}

/**
 * @brief �����ű��в����¹��̱�ʶ��
 * @param name ����
 * @param pos ���̵���ʼλ��
 * @param type ��������
*/
void IDTable::insertProc(String name, String type, int pos, bool isOuter) {
	if (isOuter) {
		table[name] = ValTable{ type, int(next.size() - 1), true };
	}
	else {
		table[name] = ValTable{ type, -1, true };
		position = pos;
	}
}

/**
 * @brief Ѱ�ҷ���
 * @param name ����
 * @param recall �Ƿ���ݲ���
 * @return -1��û�ң����ҵ���������Ե�ַ
*/
int IDTable::find(String name, bool recall) {
	IDTable* p = this;

	while (p) {
		auto val = p->table.find(name);
		if (val != p->table.end())
			return (*val).second.offset;

		if (!recall) break;
		p = p->getPrevious();
	}
	return -1;
}

/**
 * @brief Ѱ�Һ�������
 * @param name ����
 * @return -100��û�ҵ������ҵ��������Ӧ��nextָ��λ��
*/
int IDTable::findProcess(String name) {
	auto p = table.find(name);
	if (p == table.end()) return -100;
	if (!(*p).second.isFunction) return -100;
	return (*p).second.offset;
}

/**
 * @brief Ѱ�Һ������̵���ʼ��ַ
 * @param name ����
 * @return �������̵���ʼ��ַ
*/
int IDTable::findProcessPosition(String name) {
	IDTable* p = this;
	while (p) {
		auto value = p->table.find(name);
		if (value != p->table.end() && value->second.isFunction) {
			if (value->second.offset == -1) {
				return p->position;
			}
			else {
				return p->next[value->second.offset]->position;
			}
		}
		p = p->getPrevious();
	}
}

/**
 * @brief ��ȡ��ʶ��������
 * @param name ����
 * @return ����
*/
String IDTable::getType(String name) {
	auto p = table.find(name);
	if (p == table.end()) return "";
	return (*p).second.type;
}

IDTable* IDTable::getPrevious() { 
	return previous; 
}

IDTable* IDTable::getNext(int num) {
	return next[num];
}

void IDTable::addNext(IDTable* next_table) {
	next.push_back(next_table);
}

/**
 * @brief ��Ӳ������÷��ű��ʾһ��������
 * @param type ��������
*/
void IDTable::addParameter(String type) {
	parameters.push_back(type);
}

/**
 * @brief �����ú���ʱ�������Ƿ�ƥ��
 * @param type ʵ������
 * @return �Ƿ�ƥ��
*/
bool IDTable::checkParameters(String type) {
	std::vector<String> args;
	size_t start = 0, end = type.find(";", start);

	while (end != type.npos) {
		args.push_back(type.substr(start, end - start));
		start = end + 1;
		if (start >= type.size()) break;
		end = type.find(";", start);
	}
	int size = args.size();
	if (parameters.size() != size) return false;

	for (int i = 0; i < size; i++) {
		if (parameters[i] != args[i]) return false;
	}
	return true;
}

IDTable::~IDTable() {
	for (IDTable* n : next) {
		delete n;
	}
}

/*********************************************************************/



/****************************�����������Ա����*************************/

/**
 * @brief �������������ļ�������Ӧ�����嶯��
 * @param grammer ���е��ķ�
 * @param file_name ���������ļ�
*/
void SemanticAnalyzer::generateSemanticAction(Grammer* grammer, const char* file_name) {
	// ��ȡ��������������ļ� Semantic.txt
	std::ifstream semanticFile(file_name);
	if (!semanticFile.is_open()) {
		std::cout << "fail to open semantic file!" << std::endl;
		exit(0);
	}

	String line;
	// ���ж���
	while (getline(semanticFile, line)) {
		size_t start_pos = 0, end_pos = 0;
		if (line == "$$") {
			semanticActions.push_back(std::vector<SemanticAction>());
			continue;
		}

		// �и��ÿһ�����嶯����
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

			// �ָ��ĳһ�����嶯��
			while (edp != str_action.npos && stp < str_action.size()) {
				edp = str_action.find(";", stp);
				String str_subaction = str_action.substr(stp, edp - stp);

				size_t equal_pos = str_subaction.find("=");
				size_t func_pos = str_subaction.find("(");

				// ��Ϊ�����������һ���ú�������һ��ֵ�����߽��
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
 * @brief ����ÿһ���ķ�����Ӧ��Ҫ��Ӧ�����Է���
 * @param file_name 
*/
void SemanticAnalyzer::distributeAttributeSymbols(const char* file_name) {
	// ��ȡ���������������ļ� AttributeSymbol.txt
	std::ifstream fin(file_name);
	if (!fin.is_open()) {
		std::cout << "fail to open attribute file!" << std::endl;
		exit(0);
	}

	String line;
	size_t pos;
	// ���ж���
	while (getline(fin, line)) {
		pos = line.find(" ");
		symbolDistribute[line.substr(0, pos)] = line.substr(pos + 1);
	}
}

/**
 * @brief ����������Ե��ķ�����
 * @param symbol ����
 * @return �����Ե��ķ�����
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
 * @brief ��ƫ�����ӷ���ջ��ȡ�ķ�����
 * @param index ƫ����
 * @return �ķ�����
*/
Symbol* SemanticAnalyzer::getSymbolFromStack(int index) {
	if (index > reductionCount) {
		return reductionResult;
	}
	return stateStack[stateStack.size() - index];
}

/**
 * @brief ����м����
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

	// ִ��������Ŷ�Ӧ���������
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

void SemanticAnalyzer::emite(String op, String arg1, String arg2, String res) {
	Variable var1, var2, target;
	if (op == "jal") {
		res = std::to_string(nowTable->findProcessPosition(res));
	}
	var1.name = arg1;
	var2.name = arg2;
	target.name = res;

	intermediateCode.push_back(Quaternion(op, var1, var2, target));
}

int SemanticAnalyzer::nextstat(int next) {
	return intermediateCode.size() + next - 1;
}

void SemanticAnalyzer::backpatch(int index, String addr) {
	intermediateCode[index].setResult(Variable{ addr, 0 });
}

String SemanticAnalyzer::newtemp() {
	
	return String("#T") + std::to_string(newTempCount++);
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
	nowTable->insertProc(name, type, std::stoi(pos), arg == "1");
	if (arg == "0" && name == "main") {
		backpatch(0, std::to_string(nextstat(1)));
	}
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

SemanticAnalyzer::~SemanticAnalyzer() {
	delete nowTable;
}

/*********************************************************************/


/******************************��Ԫʽ��Ա����**************************/

Quaternion::Quaternion(String op, Variable arg1, Variable arg2, Variable tartget) {
	operate = op;
	parameter1 = arg1;
	parameter2 = arg2;
	result = tartget;
}

void Quaternion::setResult(Variable result) {
	this->result = result;
}

void Quaternion::output(std::ostream& out) {
	out << std::setw(8) << operate << " " << std::setw(8) << parameter1.name << " " << std::setw(8) << parameter2.name << " " << std::setw(8) << result.name << std::endl;
}

/*********************************************************************/