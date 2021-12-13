#ifndef REFLECTBASE_H
#define REFLECTBASE_H

#include <unordered_map>
#include <string>

struct ReflectItem {
	std::string type;
	char* data;
};

// ʵ�ַ���Ľӿ�
class ReflectBase {
protected:
	std::unordered_map<std::string, ReflectItem> reflectMap;
public:
	virtual void setupReflect() = 0;		// ����������Ҫ����Ϣ
	char* getFields(std::string);			// ��ȡ��Ա����
	std::string getFieldType(std::string);	// ��ȡ��Ա����������
	void setFields(std::string, char*);		// �޸ĳ�Ա����
};

#endif // !REFLECTBASE_H
