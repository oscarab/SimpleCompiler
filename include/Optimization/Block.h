#ifndef BLOCK
#define BLOCK

#include <vector>
#include <unordered_map>
#include "Semantic/Quaternion.h"

using std::string;

class Node {
private:
	std::vector<string> signs;
	string operate;		// ������
	int left;			// ����
	int right;			// �Һ���
public:
	Node(string sign) : left(-1), right(-1), operate("") { signs.push_back(sign); }

	int getLeft() { return left; }
	int getRight() { return right; }
	void setLeft(int left) { this->left = left; }
	void setRight(int right) { this->right = right; }

	string getOperator() { return operate; }
	void setOperator(string op) { operate = op; }
	void remove(string sign) { signs.erase(std::find(signs.begin(), signs.end(), sign)); }
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
