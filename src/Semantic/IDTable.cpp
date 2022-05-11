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
 * @brief �����ű��в����·���
 * @param name ����
 * @param type ����
 * @param w ���
*/
void IDTable::insert(String name, String type, int w) {
	table[name] = ValTable{ type, width, 0 };
	width += w;
}

/**
 * @brief �����ű��в����¹��̱�ʶ��
 * @param name ����
 * @param pos ���̵���ʼλ��
 * @param type ��������
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
 * @brief �����ű��в������������
 * @param name ����
 * @param type ����
 * @param w ���
 * @param dim ά��
*/
void IDTable::insertArray(String name, String type, std::vector<int>& dim_info, int w, int dim) {
	table[name] = ValTable{ type, width, dim , dim_info };
	width += w;
}

/**
 * @brief Ѱ�ҷ���
 * @param name ����
 * @param recall �Ƿ���ݲ���
 * @return -1��û�ң����ҵ���������Ե�ַ
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
 * @brief Ѱ�Һ�������
 * @param name ����
 * @return -100��û�ҵ������ҵ��������Ӧ��nextָ��λ��
*/
int IDTable::findProcess(String name) {
	auto p = table.find(name);
	if (p == table.end()) return -100;
	if ((*p).second.valType != 1) return -100;
	return (*p).second.offset;
}

/**
 * @brief Ѱ�Һ������̵���ʼ��ַ
 * @param name ����
 * @return �������̵���ʼ��ַ
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
 * @brief Ѱ�������ά��
 * @param name ����
 * * @param dim ά����
 * @return ά�ȶ�Ӧ����
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
 * @brief ��ȡ��ʶ��������
 * @param name ����
 * @return ����
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
 * @brief ��Ӳ������÷��ű��ʾһ��������
 * @param type ��������
*/
void IDTable::addParameter(String type) {
	parameters.push_back(type);
}

/**
 * @brief �����ú���ʱ�������Ƿ�ƥ��
 * @param type ʵ������
 * @return �Ƿ�ƥ��
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