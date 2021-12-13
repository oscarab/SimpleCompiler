#ifndef REFLECTBASE_H
#define REFLECTBASE_H

#include <unordered_map>
#include <string>

struct ReflectItem {
	std::string type;
	char* data;
};

// 实现反射的接口
class ReflectBase {
protected:
	std::unordered_map<std::string, ReflectItem> reflectMap;
public:
	virtual void setupReflect() = 0;		// 建立反射需要的信息
	char* getFields(std::string);			// 获取成员变量
	std::string getFieldType(std::string);	// 获取成员变量的类型
	void setFields(std::string, char*);		// 修改成员变量
};

#endif // !REFLECTBASE_H
