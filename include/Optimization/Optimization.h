#ifndef OPTIMIZATION
#define OPTIMIZATION

#include "Block.h"

using std::vector;

class Optimization {
private:
	vector<Block> blocks;

public:
	void generateLabel(vector<Quaternion>&);
	void splitBlocks(vector<Quaternion>&);
	void optimize();
	vector<Block>& getBlocks();
};

#endif // !OPTIMIZATION
