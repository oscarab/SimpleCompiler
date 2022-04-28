#include "Optimization/Optimization.h"
#include "Optimization/Block.h"

using std::string;

void Block::append(Quaternion quaternion) {
	innerCode.push_back(quaternion);
}

int isNumber(string text) {
	if ((text[0] >= '0' && text[0] <= '9') || text[0] == '-') {
		return text.find('.') != text.npos ? 2 : 1;
	}
	return 0;
}

double compute(double x, double y, string op) {
	if (op == "+") {
		return x + y;
	}
	else if (op == "-") {
		return x - y;
	}
	else if (op == "*") {
		return x * y;
	}
	else {
		return x / y;
	}
}

/*
* @brief ʹ��DAG���п����Ż�
*/
void Block::optimize() {
	for (Quaternion& quaternion : innerCode) {
		if (quaternion.getOperator() == ":="
			|| quaternion.getOperator() == "+" || quaternion.getOperator() == "-"
			|| quaternion.getOperator() == "*" || quaternion.getOperator() == "/") {

			int n = 0;	// ���A��Ҫ���ϵĽ��ı��

			bool createB = false, createC = false;
			Variable b = quaternion.getParameter(1);
			Variable target = quaternion.getResult();
			auto p = nodeMapping.find(b.name);
			if (p == nodeMapping.end()) {
				// ���NODE(B)�����ھ��½�һ�����
				nodes.push_back(Node(b.name));
				nodeMapping[b.name] = nodes.size() - 1;
				createB = true;
				n = nodes.size() - 1;
			}
			else n = p->second;

			// �����2���м����
			if (quaternion.getOperator() != ":=") {
				Variable c = quaternion.getParameter(2);
				if (nodeMapping.find(c.name) == nodeMapping.end()) {
					// ���NODE(C)�����ھ��½�һ�����
					nodes.push_back(Node(c.name));
					nodeMapping[c.name] = nodes.size() - 1;
					createC = true;
				}
				
				int btype = isNumber(b.name), ctype = isNumber(c.name);
				string op = quaternion.getOperator();
				if (btype && ctype) {
					// ����������ǳ����Ϳ��Ժϲ�
					double result = compute(std::stod(b.name), std::stod(c.name), op);
					if (btype == 1 && ctype == 1) {
						result = (long long)result;
					}

					// ���B��C������´����ľ�ȥ��
					if (createB) {
						nodes.pop_back();
						nodeMapping.erase(b.name);
					}
					if (createC) {
						nodes.pop_back();
						nodeMapping.erase(c.name);
					}

					string new_name = std::to_string(result);
					// ����ϲ���Ľ�㲻����Ҳ�½�һ��
					if (nodeMapping.find(new_name) == nodeMapping.end()) {
						nodes.push_back(Node(new_name));
						nodeMapping[new_name] = nodes.size() - 1;
					}
					n = nodeMapping[new_name];
				}
				else {
					// ����ͼ���Ƿ��й����ӱ���ʽ
					int bindex = nodeMapping[b.name], cindex = nodeMapping[c.name];
					bool has_common = false;
					if (!createB && !createC)
						for (int i = 0; i < nodes.size(); i++) {
							Node& node = nodes[i];
							if (node.getLeft() == bindex && node.getRight() == cindex && node.getOperator() == op) {
								n = i;
								has_common = true;
							}
						}

					// û�й����ӱ���ʽ����Ҫ�½����
					if (!has_common) {
						Node node(target.name);
						node.setOperator(op);
						node.setLeft(bindex);
						node.setRight(cindex);
						nodes.push_back(node);
						n = nodes.size() - 1;
					}
				}
			}

			p = nodeMapping.find(target.name);
			if (p == nodeMapping.end()) {
				// ���NODE(A)��������A�������n
				nodeMapping[target.name] = n;
			}
			else {
				// �����Ƚ�A��ԭ���Ľ����ȥ��
				if (!nodes[p->second].isLeaf())
					nodes[p->second].remove(target.name);
				nodeMapping[target.name] = n;
			}
		}
	}
}

/*
* @brief ���м���뻮�ֻ�����
*/
void Optimization::splitBlocks(vector<Quaternion>& quaternions) {
	vector<int> enter_flag(quaternions.size() + 1);
	
	int q_size = quaternions.size();
	for (int i = 0; i < q_size; i++) {
		Quaternion& quaternion = quaternions[i];
		string op = quaternion.getOperator();

		// ��ת�����ת����λ�ö������
		if (op == "j") {
			enter_flag[quaternion.getAddress()] |= 1;
			enter_flag[i] |= 2;	// ���Ϊ������ת�����
		}
		else if (op == "jal") {
			enter_flag[quaternion.getAddress()] |= 1;
			enter_flag[i] |= 2;
			enter_flag[i + 1] |= 1;
		}
		else if (op == "jr") {
			enter_flag[i] |= 2;
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

			if (enter_flag[j] & 1) {
				// �����һ���������䣬������
				j--;
				break;
			}
			else if (enter_flag[j] & 2) {
				// �����һ����ת����䣬����
				block.append(quaternions[j]);
				break;
			}
		}
		blocks.push_back(block);
		i = j;
	}
}

void Optimization::optimize() {
	for (Block& block : blocks) {
		block.optimize();
	}
}