#pragma once
#include "components/ComponentTypes.h"
#include <vector>
struct Entity {
	unsigned int UID = 0;
	unsigned int ComponentBitfield = 0;
	std::vector<int> Components;
};