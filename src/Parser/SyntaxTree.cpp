#include "Parser/Parser.h"
#include <iostream>

extern void tab(std::ostream& out, int level);

/**
 * @brief 获取语法树
 * @return 语法树
*/
SyntaxTree* Parser::getTree() {
	return &tree;
}

/**
 * @brief 语法树结点构造
 * @param sym 符号
*/
SyntaxNode::SyntaxNode(Symbol* sym) {
	symbol = sym;
}

/**
 * @brief 给结点增加孩子
 * @param node 孩子结点
*/
void SyntaxNode::addChild(SyntaxNode* node) {
	child.push_back(node);
}

/**
 * @brief 输出
 * @param out 输出源
 * @param level 缩进
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
 * @brief 获取该结点的所有孩子
 * @return 所有孩子
*/
NodeList* SyntaxNode::getChildren() {
	return &child;
}

/**
 * @brief 获取结点代表的符号
 * @return 符号
*/
Symbol* SyntaxNode::getSymbol() {
	return symbol;
}

/**
 * @brief 插入新的结点
 * @param symbol 符号
*/
void SyntaxTree::insert(Symbol* symbol) {
	SyntaxNode* node = new SyntaxNode(symbol);

	constructStack.push_back(node);
}

/**
 * @brief 插入新结点，并将构造栈中的若干结点作为新结点的孩子
 * @param symbol 符号
 * @param size 子结点数量
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
 * @brief 获取根结点
 * @return 根结点
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