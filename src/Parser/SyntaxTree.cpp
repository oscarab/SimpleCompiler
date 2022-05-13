#include "Parser/Parser.h"
#include "Output/Log.h"
#include <iostream>

extern void tab(Log* log, int level);

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
 * @param level ����
*/
void SyntaxNode::write(int level, char end) {
	Log* log = FileLog::getInstance("tree.txt");
	tab(log, level);
	log->logln("{");

	symbol->write(log, level + 1);

	tab(log, level + 1);
	log->log("\"child\": [");

	int size = child.size();
	log->logln((size > 0 ? "" : "]"));
	for (int i = 0; i < size; i++) {
		SyntaxNode* node = child[i];

		if (i < size - 1) {
			node->write(level + 2, ',');
		}
		else {
			node->write(level + 2, ']');
		}
	}

	tab(log, level);
	log->logln("}" + end);
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