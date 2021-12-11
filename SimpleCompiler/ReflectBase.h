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
	virtual void setupReflect() = 0;
	char* getFields(std::string);
	std::string getFieldType(std::string);
	void setFields(std::string, char*);
};

#endif // !REFLECTBASE_H
