#pragma once
int GenerateTypeID() {
	static int value = 0;
	return value++;
}
int CreateBitFlag(int type) {
	return (1 << type);
}