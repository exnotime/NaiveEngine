#pragma once
static int GenerateTypeID() {
	static int value = 0;
	return value++;
}
template <typename t>
static int GetComponentTypeID() {
	static int type = GenerateTypeID();
	return type;
}

static int CreateBitFlags(int type) {
	return (1 << type);
}