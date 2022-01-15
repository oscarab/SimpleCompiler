#include "Parser/Parser.h"
#include <iostream>

extern void tab(std::ostream& out, int level);

/**
 * @brief ��ȡ�﷨��
 * @return �﷨��
*/
SyntaxTree* Parser::getTree() {
	return &tree;
}

/**
 * @brief �﷨����㹹��
 * @param sym ����
*/
SyntaxNode::SyntaxNode(Symbol* sym) {
	symbol = sym;
}

/**
 * @brief ��������Ӻ���
 * @param node ���ӽ��
*/
void SyntaxNode::addChild(SyntaxNode* node) {
	child.push_back(node);
}

/**
 * @brief ���
 * @param out ���Դ
 * @param level ����
*/
void SyntaxNode::write(std::ostream& out, int level, char end) {
	tab(out, level);
	out << "{" << std::endl;

	symbol->write(out, level + 1);

	tab(out, level + 1);
	out << "\"child\": [";

	int size = child.size();
	out << (size > 0 ? "" : "]") << std::endl;
	for (int i = 0; i < size; i++) {
		SyntaxNode* node = child[i];

		if (i < size - 1) {
			node->write(out, level + 2, ',');
		}
		else {
			node->write(out, level + 2, ']');
		}
	}

	tab(out, level);
	out << "}" << end << std::endl;
}

/**
 * @brief ��ȡ�ý������к���
 * @return ���к���
*/
NodeList* SyntaxNode::getChildren() {
	return &child;
}

/**
 * @brief ��ȡ������ķ���
 * @return ����
*/
Symbol* SyntaxNode::getSymbol() {
	return symbol;
}

/**
 * @brief �����µĽ��
 * @param symbol ����
*/
void SyntaxTree::insert(Symbol* symbol) {
	SyntaxNode* node = new SyntaxNode(symbol);

	constructStack.push_back(node);
}

/**
 * @brief �����½�㣬��������ջ�е����ɽ����Ϊ�½��ĺ���
 * @param symbol ����
 * @param size �ӽ������
*/
void SyntaxTree::construct(Symbol* symbol, int size) {
	int len = constructStack.size();
	SyntaxNode* node = new SyntaxNode(symbol);

	for (int i = len - size; i < len && len > 0; i++) {
		node->addChild(constructStack[i]);
	}

	while (size-- && len > 0) {
		constructStack.pop_back();
	}

	constructStack.push_back(node);

	if (constructStack.size() == 1) {
		head = node;
	}
}

/**
 * @brief ��ȡ�����
 * @return �����
*/
SyntaxNode* SyntaxTree::getHead() {
	return head;
}

void deleteNode(SyntaxNode* node) {
	if (node == NULL)
		return;

	for (SyntaxNode* n : *node->getChildren()) {
		deleteNode(n);
	}

	if (node->getSymbol()->isEnd()) {
		Terminator* terminator = static_cast<Terminator*>(node->getSymbol());
		if (terminator->getToken().getIndex() != -1) {
			delete node->getSymbol();
		}
	}
	delete node;
}

SyntaxTree:: ~SyntaxTree() {
	deleteNode(head);
}