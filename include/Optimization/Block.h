#ifndef BLOCK
#define BLOCK

#include <vector>
#include <unordered_map>
#include "Semantic/Quaternion.h"

using std::string;

class Node {
private:
	std::vector<Variable> signs;
	string operate;		// ������
	int left;			// ����
	int right;			// �Һ���
public:
	Node(Variable sign) : left(-1), right(-1), operate("") { signs.push_back(sign); }
	Node() : left(-1), right(-1), operate("") {}

	int getLeft() { return left; }
	int getRight() { return right; }
	void setLeft(int left) { this->left = left; }
	void setRight(int right) { this->right = right; }
	std::vector<Variable>& getSigns() { return signs; }

	string getOperator() { return operate; }
	void setOperator(string op) { operate = op; }
	void remove(string sign) {
		int pos;
		for (pos = 0; pos < signs.size(); pos++) {
			if (signs[pos].name == sign) break;
		}
		signs.erase(signs.begin() + pos);
	}
	void addSign(Variable sign) { signs.push_back(sign); }
	bool isLeaf() { return left == -1 && right == -1; }
};

class Block {
private:
	std::vector<Quaternion> innerCode;				// �м����
	std::vector<Node> nodes;						// DAGͼ�еĽ��
	std::unordered_map<string, int> nodeMapping;	// ���Ŷ�Ӧ������ӳ�亯��
public:
	void append(Quaternion);
	void optimize();
};

#endif // !BLOCK
