#include "Generator/Generator.h"


const string Instruction[] = {"add", "sub", "addi", "subi", "mul", "div",
								"and", "or",
								"lw", "sw", "j", "jal", "jr", "beq", 
								"bne", "slt", "nop"};
enum InstructionName
{
	ADD, SUB, ADDI, SUBI, MUL, DIV, AND, OR, LW, SW, J, JAL, JR, BEQ, BNE, SLT, NOP
};
const unsigned int START = 268500992;

/*
* @brief ��ʼ������������
*/
void Generator::init(unsigned int global) {
	for (int i = 9; i >= 0; i--) {
		free.push_back(Register{ "$t" + std::to_string(i) });
	}
	addInstruction(Instruction[ADDI] + "\t$gp,\t$zero,\t" + std::to_string(START));
	addInstruction(Instruction[ADDI] + "\t$fp,\t$zero,\t" + std::to_string(START + global));
	addInstruction(Instruction[ADDI] + "\t$sp,\t$zero,\t" + std::to_string(START + global + 4));
	addInstruction(Instruction[ADDI] + "\t$s0,\t$zero,\t1");
	addInstruction(Instruction[ADDI] + "\t$s1,\t$zero,\t2");
	addInstruction(Instruction[ADDI] + "\t$s2,\t$zero,\t3");
	addInstruction(Instruction[ADDI] + "\t$s3,\t$zero,\t4");
	addInstruction(Instruction[ADDI] + "\t$s4,\t$zero,\t5");
}

/*
* @brief ����һ�����ָ��
*/
void Generator::addInstruction(const string& instr) {
	if (label != "")
		assembly.push_back(label + ":\t" + instr);
	else
		assembly.push_back(instr);
	label = "";
}

/*
* �ͷ�����
*/
void Generator::freeAll() {
	while (!occupy.empty()) {
		Register& regist = occupy.back();
		Variable& var = regist.var;

		if (var.name.find("ARRAY/") != var.name.npos) {
			// ��һ�������������Ҫ��ô洢�±�ļĴ���
			string index_reg = getArrayIndexReg(var);
			addInstruction(Instruction[SW] + "\t" + regist.reg + ",\t" + index_reg);
			if (occupy.back().reg != regist.reg) {
				free.push_back(occupy.back());
				occupy.pop_back();
			}
		}
		else {
			if (!std::isdigit(var.name[0]) && var.name[0] != '#')
				addInstruction(Instruction[SW] + "\t" + regist.reg + ",\t" + getVariableAddress(var.address));
		}

		free.push_back(occupy.back());
		occupy.pop_back();
	}
}

/*
* ��ȡ����Ԫ�ص��±�Ĵ���
*/
string Generator::getArrayIndexReg(Variable& var) {
	int first = var.name.find_first_of("/");
	int last = var.name.find_last_of("/");
	string offset_val = var.name.substr(last + 1);
	int offset_val_addr = std::stoi(var.name.substr(first + 1, last - first - 1));
	string base = var.address <= -100 ? std::to_string(-var.address - 100) : std::to_string(var.address + 8);

	string has_in_reg = findInRegsiter(Variable{ offset_val, offset_val_addr });
	if (has_in_reg != "" && index_handled.count(offset_val))
		return base + "(" + has_in_reg + ")";

	// ��ȡһ���Ĵ����洢�����±�
	string index_reg = findValue(Variable{ offset_val, offset_val_addr });
	if (index_handled.count(offset_val)) return base + "(" + index_reg + ")";

	addInstruction(Instruction[MUL] + "\t" + index_reg + ",\t" + index_reg + ",\t$s3");
	if (var.address <= -100) {
		addInstruction(Instruction[ADD] + "\t" + index_reg + ",\t" + index_reg + ",\t$gp");
	}
	else {
		addInstruction(Instruction[ADD] + "\t" + index_reg + ",\t" + index_reg + ",\t$fp");
	}
	index_handled.insert(offset_val);
	return base + "(" + index_reg + ")";
}

/*
* ������Ե�ַ��ȡ����λ��
*/
string Generator::getVariableAddress(int addr) {
	if (addr >= 0) {
		return std::to_string(addr + 8) + "($fp)";
	}
	else {
		return std::to_string(-addr - 100) + "($gp)";
	}
}

string Generator::findInRegsiter(Variable& var) {
	int occ_size = occupy.size();
	for (int i = 0; i < occ_size; i++) {
		// �����Ѿ��ڼĴ�����
		if (occupy[i].var.name == var.name) {
			locked.insert(occupy[i].reg);
			return occupy[i].reg;
		}
	}
	return "";
}

/*
* @brief Ѱ�ҿ��еļĴ��������ñ���
*/
string Generator::findFreeRegister(Variable& var) {
	static int find_start = 0;
	if (free.size() > 0) {
		// ������п��мĴ�����ֱ��ȡ����ʹ��
		occupy.push_back(free.back());
		free.pop_back();
		occupy.back().var = var;
		locked.insert(occupy.back().reg);
		return occupy.back().reg;
	}
	else {
		int temp_place = -1;   // ���Ҫ��ռ�ļĴ���
		for (int i = 0; i < 10; i++) {
			if (std::isdigit(occupy[i].var.name[0])) {
				temp_place = i;
				break;
			}
		}
		if (temp_place == -1) {
			for (int i = (find_start + 1) % 10; i != find_start; i = (i + 1) % 10) {
				if (locked.count(occupy[i].reg) == 0) {
					temp_place = i;
					break;
				}
			}
			find_start = temp_place;
		}
		Variable& old_var = occupy[temp_place].var;

		// ������ռ�ı����浽�ڴ���
		if (old_var.name.find("ARRAY/") != old_var.name.npos) {
			string index_reg = getArrayIndexReg(old_var);
			addInstruction(Instruction[SW] + "\t" + occupy[temp_place].reg + ",\t" + index_reg);
		}
		else if (!std::isdigit(old_var.name[0])) {
			addInstruction(Instruction[SW] + "\t" + occupy[temp_place].reg + ",\t" + getVariableAddress(old_var.address));
		}
		
		occupy[temp_place].var = var;			// ����Ҫ�ı��������Ĵ���
		locked.insert(occupy[temp_place].reg);	// ��ʱ����
		return occupy[temp_place].reg;
	}
}

/*
* @brief Ѱ�ұ������ڵļĴ���λ��
*/
string Generator::findValue(Variable& var, bool load) {
	string has_in_reg = findInRegsiter(var);
	if (has_in_reg != "") return has_in_reg;

	if (var.name.find("ARRAY/") != var.name.npos) {
		// ��һ�������������Ҫ���⴦��
		string index_reg = getArrayIndexReg(var);
		string reg = findFreeRegister(var);
		addInstruction(Instruction[LW] + "\t" + reg + ",\t" + index_reg);
		return reg;
	}

	if (var.name == "0") return "$zero";
	if (var.name == "1") return "$s0";
	if (var.name == "2") return "$s1";
	if (var.name == "3") return "$s2";
	if (var.name == "4") return "$s3";
	if (var.name == "5") return "$s4";
	if (var.name == "$v0") return "$v0";
	string reg = findFreeRegister(var);
	if (std::isdigit(var.name[0])) {
		addInstruction(Instruction[ADDI] + "\t" + reg + ",\t$zero,\t" + var.name);
	}
	else if (load) {
		addInstruction(Instruction[LW] + "\t" + reg + ",\t" + getVariableAddress(var.address));
	}
	return reg;
}

/*
* @brief ���м��������Mips���
*/
void Generator::generateBatch(vector<Quaternion>& quaternions, unordered_set<string>& entry) {
	int push = 0;
	label = quaternions[0].getLabel();
	if (entry.count(label)) {
		addInstruction(Instruction[SW] + "\t$ra,\t4($fp)");
	}
	for (Quaternion& quaternion : quaternions) {
		string op = quaternion.getOperator();
		locked.clear();

		if (op == "j") {
			freeAll();
			addInstruction(op + "\t" + quaternion.getResult().name);
			addInstruction(Instruction[NOP]);
		}
		else if (op == "jal") {
			freeAll();
			addInstruction(Instruction[SW] + "\t$fp,\t4($sp)");			// ���浱ǰ����ջ֡
			addInstruction(Instruction[ADDI] + "\t$fp,\t$sp,\t4");		// ����ջ֡��ջ��ָ��
			addInstruction(Instruction[ADDI] + "\t$sp,\t$sp,\t" + std::to_string(push + 4));
			addInstruction(op + "\t" + quaternion.getResult().name);	// ���ú���
			addInstruction(Instruction[NOP]);
			push = 0;
		}
		else if (op == "par") {
			string reg = findValue(quaternion.getResult());
			addInstruction(Instruction[SW] + "\t" + reg + ",\t" + std::to_string(push + 12) + "($sp)");
			push += 4;
		}
		else if (op == "jr") {
			freeAll();
			addInstruction(Instruction[LW] + "\t$ra,\t4($fp)");			// ��ȡ���ص�ַ
			addInstruction(Instruction[SUBI] + "\t$sp,\t$fp,\t4");		// ����ջ��ָ��
			addInstruction(Instruction[LW] + "\t$fp,\t0($fp)");			// ���˵���һ��ջ֡
			addInstruction(op + "\t$ra");
			addInstruction(Instruction[NOP]);
		}
		else if (op == "dec") {
			addInstruction(Instruction[ADDI] + "\t$sp,\t$sp,\t" + quaternion.getResult().name);
		}
		else if (op == ">") {
			string reg1 = findValue(quaternion.getParameter(1));
			string reg2 = findValue(quaternion.getParameter(2));
			freeAll();
			addInstruction(Instruction[SLT] + "\t$at,\t" + reg2 + ",\t" + reg1);
			addInstruction(Instruction[BNE] + "\t$zero,\t$at,\t" + quaternion.getResult().name);
			addInstruction(Instruction[NOP]);
		}
		else if (op == "<") {
			string reg1 = findValue(quaternion.getParameter(1));
			string reg2 = findValue(quaternion.getParameter(2));
			freeAll();
			addInstruction(Instruction[SLT] + "\t$at,\t" + reg1 + ",\t" + reg2);
			addInstruction(Instruction[BNE] + "\t$zero,\t$at,\t" + quaternion.getResult().name);
			addInstruction(Instruction[NOP]);
		}
		else if (op == "==") {
			string reg1 = findValue(quaternion.getParameter(1));
			string reg2 = findValue(quaternion.getParameter(2));
			freeAll();
			addInstruction(Instruction[BEQ] + "\t" + reg1 + ",\t" + reg2 + ",\t" + quaternion.getResult().name);
			addInstruction(Instruction[NOP]);
		}
		else if (op == "!=") {
			string reg1 = findValue(quaternion.getParameter(1));
			string reg2 = findValue(quaternion.getParameter(2));
			freeAll();
			addInstruction(Instruction[BNE] + "\t" + reg1 + ",\t" + reg2 + ",\t" + quaternion.getResult().name);
			addInstruction(Instruction[NOP]);
		}
		else if (op == ">=") {
			string reg1 = findValue(quaternion.getParameter(1));
			string reg2 = findValue(quaternion.getParameter(2));
			freeAll();
			addInstruction(Instruction[SLT] + "\t$at,\t" + reg1 + ",\t" + reg2);
			addInstruction(Instruction[BEQ] + "\t$zero,\t$at,\t" + quaternion.getResult().name);
			addInstruction(Instruction[NOP]);
		}
		else if (op == "<=") {
			string reg1 = findValue(quaternion.getParameter(1));
			string reg2 = findValue(quaternion.getParameter(2));
			freeAll();
			addInstruction(Instruction[SLT] + "\t$at,\t" + reg2 + ",\t" + reg1);
			addInstruction(Instruction[BEQ] + "\t$zero,\t$at,\t" + quaternion.getResult().name);
			addInstruction(Instruction[NOP]);
		}
		else if (op == "+") {
			string reg1 = findValue(quaternion.getParameter(1));
			string reg2 = findValue(quaternion.getParameter(2));
			string result = findValue(quaternion.getResult(), false);
			addInstruction(Instruction[ADD] + "\t" + result + ",\t" + reg1 + ",\t" + reg2);
		}
		else if (op == "-") {
			string reg1 = findValue(quaternion.getParameter(1));
			string reg2 = findValue(quaternion.getParameter(2));
			string result = findValue(quaternion.getResult(), false);
			addInstruction(Instruction[SUB] + "\t" + result + ",\t" + reg1 + ",\t" + reg2);
		}
		else if (op == "*") {
			string reg1 = findValue(quaternion.getParameter(1));
			string reg2 = findValue(quaternion.getParameter(2));
			string result = findValue(quaternion.getResult(), false);
			addInstruction(Instruction[MUL] + "\t" + result + ",\t" + reg1 + ",\t" + reg2);
		}
		else if (op == "/") {
			string reg1 = findValue(quaternion.getParameter(1));
			string reg2 = findValue(quaternion.getParameter(2));
			string result = findValue(quaternion.getResult(), false);
			addInstruction(Instruction[DIV] + "\t" + result + ",\t" + reg1 + ",\t" + reg2);
		}
		else if (op == ":=") {
			string reg = findValue(quaternion.getParameter(1));
			string result = findValue(quaternion.getResult(), false);
			addInstruction(Instruction[OR] + "\t" + result + ",\t$zero,\t" + reg);
		}
	}
	if (!quaternions.back().isJump() && quaternions.back().getOperator() != "jr") {
		freeAll();
	}
}

void Generator::out() {
	for (string instr : assembly) {
		std::cout << instr << std::endl;
	}
}