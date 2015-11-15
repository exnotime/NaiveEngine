#pragma once
int GenerateTypeID();
template <typename t>
int GetComponentTypeID() {
	static int type = GenerateTypeID();
	return type;
}

int CreateBitFlag(int type);
typedef size_t HashID;
#define HASH(x) constexpr std::hash<std::string>()(x)

#define GET_TYPE_ID(x) GetComponentTypeID<x>()