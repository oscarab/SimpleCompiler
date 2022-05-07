#ifndef OPTIMIZATION
#define OPTIMIZATION

#include "Block.h"
#include <unordered_set>

using std::vector;
using std::unordered_set;

class Optimization {
private:
	vector<Block> blocks;
	unordered_set<string> functionEntry;
public:
	void generateLabel(vector<Quaternion>&);
	void splitBlocks(vector<Quaternion>&);
	void optimize();
	vector<Block>& getBlocks();
	unordered_set<string>& getFunctionEntry();
};

#endif // !OPTIMIZATION
