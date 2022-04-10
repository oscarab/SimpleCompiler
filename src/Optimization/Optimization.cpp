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

		// ��ת�����ת����λ�ö������
		if (op == "j") {
			enter_flag[quaternion.getAddress()] |= 1;
			enter_flag[i] = 3;	// ���Ϊ������ת�����
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
			// ������ת���ĺ�һ��Ҳ�����
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
				// �����һ���������䣬������
				j--;
				break;
			}
			else if (enter_flag[j] == 2) {
				// �����һ����ת����䣬����
				block.append(quaternions[j]);
				break;
			}
		}
		blocks.push_back(block);
		i = j;
	}
}
