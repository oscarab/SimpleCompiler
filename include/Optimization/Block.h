#ifndef BLOCK
#define BLOCK

#include <vector>
#include "Semantic/Quaternion.h"

class Block {
private:
	std::vector<Quaternion> innerCode;

public:
	void append(Quaternion);
	void optimize();
};

#endif // !BLOCK
