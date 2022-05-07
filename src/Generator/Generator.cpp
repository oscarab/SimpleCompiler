#include "Generator/Generator.h"


const string Instruction[] = {"add", "sub", "addi", "subi", "mul", "div",
								"and", "or",
								"lw", "sw", "j", "jal", "jr", "beq", 
								"bne", "slt", "nop"};
enum InstructionName
{
	ADD, SUB, ADDI, SUBI, MUL, DIV, AND, OR, LW, SW, J, JAL, JR, BEQ, BNE, SLT, NOP
};

/*
* @brief ��ʼ������������
*/
void Generator::init() {
	for (int i = 0; i < 10; i++) {
		free.push_back(Register{ "$t" + std::to_string(i) });
	}
	addInstruction(Instruction[ADDI] + "\t$fp,\t$zero,\t0");
	addInstruction(Instruction[ADDI] + "\t$sp,\t$zero,\t4");
	addInstruction(Instruction[ADDI] + "\t$s0,\t$zero,\t1");
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
		free.push_back(occupy.back());
		occupy.pop_back();
	}
}

/*
* @brief Ѱ�ҿ��еļĴ��������ñ���
*/
string Generator::findFreeRegister(Variable& var) {
	if (free.size() > 0) {
		// ������п��мĴ�����ֱ��ȡ����ʹ��
		occupy.push_back(free.back());
		free.pop_back();
		occupy.back().var = var;
		return occupy.back().reg;
	}
	else {
		int temp_place = -1;   // ��Ǵ�����ʱ���������ļĴ���
		for (int i = 0; i < 10; i++) {
			if (occupy[i].var.name[0] == '$' || std::isdigit(occupy[i].var.name[0])) {
				temp_place = i;
				break;
			}
		}

		if (temp_place == -1) {
			temp_place = 0;
			Variable& old_var = occupy[temp_place].var;
			string offet = std::to_string(old_var.address + 8);

			// ������ռ�ı����浽�ڴ���
			addInstruction(Instruction[SW] + "\t" + occupy[temp_place].reg + ",\t" + offet + "($fp)");
		}

		// ����Ҫ�ı��������Ĵ���
		occupy[temp_place].var = var;
		return occupy[temp_place].reg;
	}
}

/*
* @brief Ѱ�ұ������ڵļĴ���λ��
*/
string Generator::findValue(Variable& var, bool load) {
	int occ_size = occupy.size();
	for (int i = 0; i < occ_size; i++) {
		// �����Ѿ��ڼĴ�����
		if (occupy[i].var.name == var.name) 
			return occupy[i].reg;
	}

	string reg = findFreeRegister(var);
	if (std::isdigit(var.name[0])) {
		if (var.name == "0") return "$zero";
		if (var.name == "1") return "$s0";
		addInstruction(Instruction[ADDI] + "\t" + reg + ",\t$zero,\t" + var.name);
	}
	else if (load) {
		addInstruction(Instruction[LW] + "\t" + reg + ",\t" + std::to_string(var.address + 8) + "($fp)");
	}
	return reg;
}

/*
* @brief ���м��������Mips���
*/
void Generator::generateBatch(vector<Quaternion>& quaternions) {
	int push = 0;
	for (Quaternion& quaternion : quaternions) {
		string op = quaternion.getOperator();
		label = quaternion.getLabel();

		if (op == "j") {
			addInstruction(op + "\t" + quaternion.getResult().name);
			addInstruction(Instruction[NOP]);
		}
		else if (op == "jal") {
			addInstruction(Instruction[SW] + "\t$fp,\t4($sp)");			// ���浱ǰ����ջ֡
			addInstruction(op + "\t" + quaternion.getResult().name);	// ���ú���
			addInstruction(Instruction[NOP]);
			addInstruction(Instruction[ADDI] + "\t$fp,\t$sp,\t4");		// ����ջ֡��ջ��ָ��
			addInstruction(Instruction[ADDI] + "\t$sp,\t$sp,\t" + std::to_string(push + 4));
			push = 0;
		}
		else if (op == "par") {
			string reg = findValue(quaternion.getResult());
			addInstruction(Instruction[SW] + "\t" + reg + ",\t" + std::to_string(push + 12) + "($sp)");
			push += 4;
		}
		else if (op == "jr") {
			addInstruction(Instruction[LW] + "\t$ra,\t4($fp)");			// ��ȡ���ص�ַ
			addInstruction(Instruction[SUBI] + "\t$sp,\t$fp,\t4");		// ����ջ��ָ��
			addInstruction(Instruction[LW] + "\t$fp,\t0($fp)");			// ���˵���һ��ջ֡
			addInstruction(op + "\t$ra");
			addInstruction(Instruction[NOP]);

			freeAll();
		}
		else if (op == "dec") {
			addInstruction(Instruction[ADDI] + "\t$sp,\t$sp,\t" + quaternion.getResult().name);
		}
		else if (op == ">") {
			string reg1 = findValue(quaternion.getParameter(1));
			string reg2 = findValue(quaternion.getParameter(2));
			addInstruction(Instruction[SLT] + "\t$at,\t" + reg2 + ",\t" + reg1);
			addInstruction(Instruction[BNE] + "\t$zero,\t$at,\t" + quaternion.getResult().name);
			addInstruction(Instruction[NOP]);
		}
		else if (op == "<") {
			string reg1 = findValue(quaternion.getParameter(1));
			string reg2 = findValue(quaternion.getParameter(2));
			addInstruction(Instruction[SLT] + "\t$at,\t" + reg1 + ",\t" + reg2);
			addInstruction(Instruction[BNE] + "\t$zero,\t$at,\t" + quaternion.getResult().name);
			addInstruction(Instruction[NOP]);
		}
		else if (op == "==") {
			string reg1 = findValue(quaternion.getParameter(1));
			string reg2 = findValue(quaternion.getParameter(2));
			addInstruction(Instruction[BEQ] + "\t" + reg1 + ",\t" + reg2 + ",\t" + quaternion.getResult().name);
			addInstruction(Instruction[NOP]);
		}
		else if (op == "!=") {
			string reg1 = findValue(quaternion.getParameter(1));
			string reg2 = findValue(quaternion.getParameter(2));
			addInstruction(Instruction[BNE] + "\t" + reg1 + ",\t" + reg2 + ",\t" + quaternion.getResult().name);
			addInstruction(Instruction[NOP]);
		}
		else if (op == ">=") {
			string reg1 = findValue(quaternion.getParameter(1));
			string reg2 = findValue(quaternion.getParameter(2));
			addInstruction(Instruction[SLT] + "\t$at,\t" + reg1 + ",\t" + reg2);
			addInstruction(Instruction[BEQ] + "\t$zero,\t$at,\t" + quaternion.getResult().name);
			addInstruction(Instruction[NOP]);
		}
		else if (op == "<=") {
			string reg1 = findValue(quaternion.getParameter(1));
			string reg2 = findValue(quaternion.getParameter(2));
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
}

void Generator::out() {
	for (string instr : assembly) {
		std::cout << instr << std::endl;
	}
}