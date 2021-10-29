#include "Grammer.h"

bool Symbol::operator==(const Symbol& symbol) const {
	return end == symbol.end && token == symbol.token && name == symbol.name;
}