#ifndef REFLECTBASE_H
#define REFLECTBASE_H

#include <unordered_map>
#include <vector>
#include <string>

struct ReflectItem {
	std::string type;
	char* data;
};

class ReflectBase {
protected:
	std::unordered_map<std::string, std::vector<ReflectItem>> reflectMap;
public:
	virtual void setupReflect() = 0;
};

#endif // !REFLECTBASE_H
