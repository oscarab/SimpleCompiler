#include "Output/Output.h"

using std::string;
using std::ofstream;

void Output::add(const char* file) {
	outs.push_back(new ofstream(file));
}

std::ofstream& Output::operator[](int idx) {
	return *outs[idx];
}

Output::~Output() {
	for (ofstream* fout : outs) {
		fout->close();
		delete fout;
	}
}