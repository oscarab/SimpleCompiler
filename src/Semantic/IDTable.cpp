#include "Semantic/SemanticAnalyzer.h"

IDTable::IDTable(IDTable* prev, int offset) {
	previous = prev;
	width = offset;
	position = 0;
}

int IDTable::getWidth() {
	return width;
}

/**
 * @brief 往符号表中插入新符号
 * @param name 名字
 * @param type 类型
 * @param w 宽度
*/
void IDTable::insert(String name, String type, int w) {
	table[name] = ValTable{ type, width, 0 };
	width += w;
}

/**
 * @brief 往符号表中插入新过程标识符
 * @param name 名字
 * @param pos 过程的起始位置
 * @param type 返回类型
*/
void IDTable::insertProcess(String name, String type, int pos, bool isOuter) {
	if (isOuter) {
		table[name] = ValTable{ type, int(next.size() - 1), 1 };
	}
	else {
		table[name] = ValTable{ type, -1, 1 };
		position = pos;
	}
}

/**
 * @brief 往符号表中插入新数组符号
 * @param name 名字
 * @param type 类型
 * @param w 宽度
 * @param dim 维数
*/
void IDTable::insertArray(String name, String type, std::vector<int>& dim_info, int w, int dim) {
	table[name] = ValTable{ type, width, dim , dim_info };
	width += w;
}

/**
 * @brief 寻找符号
 * @param name 名字
 * @param recall 是否回溯查找
 * @return -1若没找，若找到返回其相对地址
*/
int IDTable::find(String name, bool recall) {
	IDTable* p = this;

	while (p) {
		auto val = p->table.find(name);
		if (val != p->table.end()) {
			if (p->getPrevious() == NULL)
				return -val->second.offset - 100;
			else if (p->getPrevious()->getPrevious() == NULL)
				return val->second.offset;
			else
				return p->getPrevious()->width + val->second.offset;
		}

		if (!recall) break;
		p = p->getPrevious();
	}
	return -1;
}

/**
 * @brief 寻找函数过程
 * @param name 名字
 * @return -100若没找到，若找到返回其对应的next指针位置
*/
int IDTable::findProcess(String name) {
	auto p = table.find(name);
	if (p == table.end()) return -100;
	if ((*p).second.valType != 1) return -100;
	return (*p).second.offset;
}

/**
 * @brief 寻找函数过程的起始地址
 * @param name 名字
 * @return 函数过程的起始地址
*/
int IDTable::findProcessPosition(String name) {
	IDTable* p = this;
	while (p) {
		auto value = p->table.find(name);
		if (value != p->table.end() && value->second.valType == 1) {
			if (value->second.offset == -1) {
				return p->position;
			}
			else {
				return p->next[value->second.offset]->position;
			}
		}
		p = p->getPrevious();
	}
	return -1;
}

/**
 * @brief 寻找数组的维度
 * @param name 名字
 * * @param dim 维度数
 * @return 维度对应的数
*/
int IDTable::findArray(String name, int dim) {
	IDTable* p = this;
	while (p) {
		auto value = p->table.find(name);
		if (value != p->table.end() && value->second.dimension.size() > 0) {
			int dim_size = value->second.dimension.size();
			if (dim == dim_size) return 0;
			if (dim > dim_size) return -1;
			return value->second.dimension[dim];
		}
		p = p->getPrevious();
	}
	return -1;
}

/**
 * @brief 获取标识符的类型
 * @param name 名字
 * @return 类型
*/
String IDTable::getType(String name) {
	auto p = table.find(name);
	if (p == table.end()) return "";
	return (*p).second.type;
}

IDTable* IDTable::getPrevious() {
	return previous;
}

IDTable* IDTable::getNext(int num) {
	return next[num];
}

void IDTable::addNext(IDTable* next_table) {
	next.push_back(next_table);
}

/**
 * @brief 添加参数（该符号表表示一个函数）
 * @param type 参数类型
*/
void IDTable::addParameter(String type) {
	parameters.push_back(type);
}

/**
 * @brief 检查调用函数时的类型是否匹配
 * @param type 实参类型
 * @return 是否匹配
*/
bool IDTable::checkParameters(String type) {
	std::vector<String> args;
	size_t start = 0, end = type.find(";", start);

	while (end != type.npos) {
		args.push_back(type.substr(start, end - start));
		start = end + 1;
		if (start >= type.size()) break;
		end = type.find(";", start);
	}
	int size = args.size();
	if (parameters.size() != size) return false;

	for (int i = 0; i < size; i++) {
		if (parameters[i] != args[i]) return false;
	}
	return true;
}

IDTable::~IDTable() {
	for (IDTable* n : next) {
		delete n;
	}
}