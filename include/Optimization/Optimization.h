#ifndef OPTIMIZATION
#define OPTIMIZATION

#include "Block.h"

using std::vector;

class Optimization {
private:
	vector<Block> blocks;

public:
	void splitBlocks(vector<Quaternion>&);
};

#endif // !OPTIMIZATION
