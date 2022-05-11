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

Variable toVariable(string name) {
	int first = name.find_first_of("/");
	int last = name.find_last_of("/");
	string offset_val = name.substr(last + 1);
	int offset_val_addr = std::stoi(name.substr(first + 1, last - first - 1));
	return Variable{ offset_val, offset_val_addr };
}


/*
* @brief 使用DAG进行块内优化
*/
void Block::optimize() {
	bool opt = false;
	string label = innerCode[0].getLabel();		// 记录下标签

	for (Quaternion& quaternion : innerCode) {
		if (quaternion.getOperator() == ":="
			|| quaternion.getOperator() == "+" || quaternion.getOperator() == "-"
			|| quaternion.getOperator() == "*" || quaternion.getOperator() == "/") {

			int n = 0;	// 结点A将要附上的结点的编号
			opt = true;

			bool createB = false, createC = false;
			Variable b = quaternion.getParameter(1);
			Variable target = quaternion.getResult();
			auto p = nodeMapping.find(b.name);
			if (p == nodeMapping.end()) {
				// 如果NODE(B)不存在就新建一个结点
				nodes.push_back(Node(b));
				nodeMapping[b.name] = nodes.size() - 1;
				createB = true;
				n = nodes.size() - 1;
			}
			else n = p->second;

			// 如果是2型中间代码
			if (quaternion.getOperator() != ":=") {
				Variable c = quaternion.getParameter(2);
				if (nodeMapping.find(c.name) == nodeMapping.end()) {
					// 如果NODE(C)不存在就新建一个结点
					nodes.push_back(Node(c));
					nodeMapping[c.name] = nodes.size() - 1;
					createC = true;
				}
				
				int btype = isNumber(b.name), ctype = isNumber(c.name);
				string op = quaternion.getOperator();
				if (btype && ctype) {
					// 如果两个都是常数就可以合并
					double result = compute(std::stod(b.name), std::stod(c.name), op);
					if (btype == 1 && ctype == 1) {
						result = (long long)result;
					}

					// 如果B、C结点是新创建的就去掉
					if (createB) {
						nodes.pop_back();
						nodeMapping.erase(b.name);
					}
					if (createC) {
						nodes.pop_back();
						nodeMapping.erase(c.name);
					}

					string new_name = std::to_string(result);
					// 如果合并后的结点不存在也新建一个
					if (nodeMapping.find(new_name) == nodeMapping.end()) {
						nodes.push_back(Node(Variable{ new_name }));
						nodeMapping[new_name] = nodes.size() - 1;
					}
					n = nodeMapping[new_name];
				}
				else {
					// 否则就检查是否有公共子表达式
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

					// 没有公共子表达式就需要新建结点
					if (!has_common) {
						Node node;
						node.setOperator(op);
						node.setLeft(bindex);
						node.setRight(cindex);
						nodes.push_back(node);
						n = nodes.size() - 1;
					}
				}
			}
			else if (b.name.find("ARRAY/") != string::npos) {
				// 如果是将数组元素赋值给变量
				Variable c = toVariable(b.name);
				if (nodeMapping.find(c.name) == nodeMapping.end()) {
					// 如果NODE(C)不存在就新建一个结点
					nodes.push_back(Node(c));
					nodeMapping[c.name] = nodes.size() - 1;
				}
				int bindex = nodeMapping[b.name], cindex = nodeMapping[c.name];
				Node node;
				node.setOperator("=[]");
				node.setLeft(bindex);
				node.setRight(cindex);
				nodes.push_back(node);
				n = nodes.size() - 1;
			}
			else if (target.name.find("ARRAY/") != string::npos) {
				// 如果是将变量赋值给数组元素
				Variable c = toVariable(target.name);
				if (nodeMapping.find(c.name) == nodeMapping.end()) {
					// 如果NODE(C)不存在就新建一个结点
					nodes.push_back(Node(c));
					nodeMapping[c.name] = nodes.size() - 1;
				}
				int bindex = nodeMapping[b.name], cindex = nodeMapping[c.name];
				Node node;
				node.setOperator("[]=");
				node.setLeft(cindex);
				node.setRight(bindex);
				nodes.push_back(node);
				n = nodes.size() - 1;
			}

			p = nodeMapping.find(target.name);
			if (p == nodeMapping.end()) {
				// 如果NODE(A)不存在则将A附到结点n
				nodes[n].addSign(target);
				nodeMapping[target.name] = n;
			}
			else {
				// 否则先将A从原来的结点中去除
				if (!nodes[p->second].isLeaf())
					nodes[p->second].remove(target.name);
				nodes[n].addSign(target);
				nodeMapping[target.name] = n;
			}
		}
	}

	if (opt) {
		vector<Quaternion> opt_codes;

		int declare = 0;
		for (Quaternion& quaternion : innerCode) {
			if (quaternion.getOperator() == "dec") {
				declare += quaternion.getAddress();
			}
		}
		if (declare) opt_codes.push_back(Quaternion("dec", Variable{ "_" }, Variable{ "_"}, Variable{ std::to_string(declare) }));

		// 按结点的生成顺序导出优化过的中间代码
		for (Node& node : nodes) {
			vector<Variable> vars = node.getSigns();
			if (!node.isLeaf()) {
				if (node.getOperator() == "[]=") {
					Variable right = nodes[node.getRight()].getSigns()[0];
					if (node.getLeft() == node.getRight()) {
						for (Variable v : nodes[node.getRight()].getSigns()) {
							if (v.name != right.name) {
								right = v;
								break;
							}
						}
					}
					opt_codes.push_back(Quaternion(":=", right, Variable{ "_" }, vars[0]));
				}
				else if (node.getOperator() == "=[]") {
					Variable left = nodes[node.getLeft()].getSigns()[0];
					opt_codes.push_back(Quaternion(":=", left, Variable{ "_" }, vars[0]));
				}
				else {
					Variable left = nodes[node.getLeft()].getSigns()[0];
					Variable right = nodes[node.getRight()].getSigns()[0];
					opt_codes.push_back(Quaternion(node.getOperator(), left, right, vars[0]));
				}
			}
			for (int i = 1; i < vars.size(); i++) {
				opt_codes.push_back(Quaternion(":=", vars[0], Variable{ "_" }, vars[i]));
			}
		}
		for (Quaternion& quaternion : innerCode) {
			if (quaternion.getOperator() == "par"
				|| quaternion.getOperator() == "jr" || quaternion.isJump()) {
				opt_codes.push_back(quaternion);
			}
		}
		innerCode = opt_codes;
		innerCode[0].setLabel(label);
	}
}

std::vector<Quaternion>& Block::getInnerCode() {
	return innerCode;
}

/*
* @brief 对中间代码划分基本块
*/
void Optimization::splitBlocks(vector<Quaternion>& quaternions) {
	vector<int> enter_flag(quaternions.size() + 1);
	
	int q_size = quaternions.size();
	for (int i = 0; i < q_size; i++) {
		Quaternion& quaternion = quaternions[i];
		string op = quaternion.getOperator();

		// 跳转语句跳转到的位置都是入口
		if (op == "j") {
			enter_flag[quaternion.getAddress()] |= 1;
			enter_flag[i] |= 2;	// 标记为无条件转移语句
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
			// 条件跳转语句的后一个也是入口
			enter_flag[i + 1] |= 1;
		}
	}
	enter_flag[0] = 1;
	generateLabel(quaternions);

	for (int i = 0; i < q_size; i++) {
		if ((enter_flag[i] & 1) == 0) continue;

		int j = i;
		Block block;
		while (j < q_size) {
			block.append(quaternions[j]);
			j++;

			if (enter_flag[j] & 1) {
				// 如果下一条是入口语句，不包含
				j--;
				break;
			}
			else if (enter_flag[j] & 2) {
				// 如果下一条是转移语句，包含
				block.append(quaternions[j]);
				break;
			}
		}
		blocks.push_back(block);
		i = j;
	}
}

/*
* @brief 将中间代码中的地址跳跃改为标签
*/
void Optimization::generateLabel(vector<Quaternion>& quaternions) {
	int count = 1;
	string prefix = "L";

	for (Quaternion& quaternion : quaternions) {
		if (quaternion.isJump()) {
			int addr = quaternion.getAddress();

			if (quaternions[addr].getLabel() == "") {
				string label = prefix + std::to_string(count);
				quaternion.setResult(Variable{ label });
				quaternions[addr].setLabel(label);
				count++;
			}
			else {
				quaternion.setResult(Variable{ quaternions[addr].getLabel() });
			}
			if (quaternion.getOperator() == "jal") {
				functionEntry.insert(quaternion.getResult().name);
			}
		}
	}
}

void Optimization::optimize() {
	for (Block& block : blocks) {
		block.optimize();
	}
}

vector<Block>& Optimization::getBlocks() {
	return blocks;
}

unordered_set<string>& Optimization::getFunctionEntry() {
	return functionEntry;
}