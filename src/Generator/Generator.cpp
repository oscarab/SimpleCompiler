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
* @brief 初始化代码生成器
*/
void Generator::init(unsigned int global) {
	for (int i = 9; i >= 0; i--) {
		free.push_back(Register{ "$t" + std::to_string(i) });
	}
	addInstruction(Instruction[ADDI] + "\t$gp,\t$zero,\t" + std::to_string(START));
	addInstruction(Instruction[ADDI] + "\t$fp,\t$zero,\t" + std::to_string(START + global));
	addInstruction(Instruction[ADDI] + "\t$sp,\t$zero,\t" + std::to_string(START + global + 4));
	addInstruction(Instruction[ADDI] + "\t$s0,\t$zero,\t1");
}

/*
* @brief 生成一条汇编指令
*/
void Generator::addInstruction(const string& instr) {
	if (label != "")
		assembly.push_back(label + ":\t" + instr);
	else
		assembly.push_back(instr);
	label = "";
}

/*
* 释放所有
*/
void Generator::freeAll() {
	while (!occupy.empty()) {
		free.push_back(occupy.back());

		Register& regist = occupy.back();
		if (!std::isdigit(regist.var.name[0]) && regist.var.name[0] != '#')
			addInstruction(Instruction[SW] + "\t" + regist.reg + ",\t" + getVariableAddress(regist.var.address));

		occupy.pop_back();
	}
}

/*
* 根据相对地址获取变量位置
*/
string Generator::getVariableAddress(int addr) {
	if (addr >= 0) {
		return std::to_string(addr + 8) + "($fp)";
	}
	else {
		return std::to_string(-addr - 100) + "($gp)";
	}
}

/*
* @brief 寻找空闲的寄存器来安置变量
*/
string Generator::findFreeRegister(Variable& var) {
	if (free.size() > 0) {
		// 如果仍有空闲寄存器就直接取出来使用
		occupy.push_back(free.back());
		free.pop_back();
		occupy.back().var = var;
		return occupy.back().reg;
	}
	else {
		int temp_place = -1;   // 标记存有临时变量或常量的寄存器
		for (int i = 0; i < 10; i++) {
			if (occupy[i].var.name[0] == '$' || std::isdigit(occupy[i].var.name[0])) {
				temp_place = i;
				break;
			}
		}

		temp_place = temp_place  == -1 ? 0 : temp_place;
		Variable& old_var = occupy[temp_place].var;

		// 将被抢占的变量存到内存里
		addInstruction(Instruction[SW] + "\t" + occupy[temp_place].reg + ",\t" + getVariableAddress(old_var.address));

		// 将需要的变量读到寄存器
		occupy[temp_place].var = var;
		return occupy[temp_place].reg;
	}
}

/*
* @brief 寻找变量所在的寄存器位置
*/
string Generator::findValue(Variable& var, bool load) {
	int occ_size = occupy.size();
	for (int i = 0; i < occ_size; i++) {
		// 变量已经在寄存器内
		if (occupy[i].var.name == var.name) 
			return occupy[i].reg;
	}

	if (var.name == "0") return "$zero";
	if (var.name == "1") return "$s0";
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
* @brief 由中间代码生成Mips汇编
*/
void Generator::generateBatch(vector<Quaternion>& quaternions, unordered_set<string>& entry) {
	int push = 0;
	label = quaternions[0].getLabel();
	if (entry.count(label)) {
		addInstruction(Instruction[SW] + "\t$ra,\t4($fp)");
	}
	for (Quaternion& quaternion : quaternions) {
		string op = quaternion.getOperator();

		if (op == "j") {
			freeAll();
			addInstruction(op + "\t" + quaternion.getResult().name);
			addInstruction(Instruction[NOP]);
		}
		else if (op == "jal") {
			freeAll();
			addInstruction(Instruction[SW] + "\t$fp,\t4($sp)");			// 保存当前函数栈帧
			addInstruction(Instruction[ADDI] + "\t$fp,\t$sp,\t4");		// 更新栈帧和栈顶指针
			addInstruction(Instruction[ADDI] + "\t$sp,\t$sp,\t" + std::to_string(push + 4));
			addInstruction(op + "\t" + quaternion.getResult().name);	// 调用函数
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
			addInstruction(Instruction[LW] + "\t$ra,\t4($fp)");			// 获取返回地址
			addInstruction(Instruction[SUBI] + "\t$sp,\t$fp,\t4");		// 更新栈顶指针
			addInstruction(Instruction[LW] + "\t$fp,\t0($fp)");			// 回退到上一个栈帧
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