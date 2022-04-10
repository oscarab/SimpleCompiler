#include "Optimization/Optimization.h"
#include "Optimization/Block.h"

using std::string;

void Block::append(Quaternion quaternion) {
	innerCode.push_back(quaternion);
}

void Optimization::splitBlocks(vector<Quaternion>& quaternions) {
	vector<int> enter_flag(quaternions.size() + 1);
	
	int q_size = quaternions.size();
	for (int i = 0; i < q_size; i++) {
		Quaternion& quaternion = quaternions[i];
		string op = quaternion.getOperator();

		// 跳转语句跳转到的位置都是入口
		if (op == "j") {
			enter_flag[quaternion.getAddress()] |= 1;
			enter_flag[i] = 3;	// 标记为无条件转移语句
		}
		else if (op == "jal") {
			enter_flag[quaternion.getAddress()] |= 1;
			enter_flag[i] = 3;
			enter_flag[i + 1] |= 1;
		}
		else if (op == "jr") {
			enter_flag[i] = 3;
		}
		else if (op == "<" || op == "<=" || op == ">" || op == ">=" ||
				op == "==" || op == "!=") {
			enter_flag[quaternion.getAddress()] |= 1;
			// 条件跳转语句的后一个也是入口
			enter_flag[i + 1] |= 1;
		}
	}
	enter_flag[0] = 1;

	for (int i = 0; i < q_size; i++) {
		if (enter_flag[i] != 1) continue;

		int j = i;
		Block block;
		while (j < q_size) {
			block.append(quaternions[j]);
			j++;

			if (enter_flag[j] == 1) {
				// 如果下一条是入口语句，不包含
				j--;
				break;
			}
			else if (enter_flag[j] == 2) {
				// 如果下一条是转移语句，包含
				block.append(quaternions[j]);
				break;
			}
		}
		blocks.push_back(block);
		i = j;
	}
}
