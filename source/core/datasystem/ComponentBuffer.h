#pragma once
#include <malloc.h>
#include <assert.h>
#include <vector>
typedef  unsigned char Byte;
typedef  unsigned int uint;
/*
	Components can only be of pure data types no pointers please
*/
class ComponentBuffer {
public:
	ComponentBuffer() {

	}
	~ComponentBuffer() {

	}

	void CreateBuffer(uint count, uint size) {
		m_Buffer = (Byte*)malloc(count * size);
		assert(m_Buffer != nullptr);
		m_Size = count;
		m_ComponentSize = size;
	}
	void ResizeBuffer(uint newSize) {
		m_Buffer = (Byte*)realloc(m_Buffer, newSize);
		assert(m_Buffer != nullptr);
		m_Size *= 2;
	}

	void DestroyBuffer() {
		if (m_Buffer) free(m_Buffer);
	}
	//makes a copy of the content at component
	uint AddComponent(const void* component) {
		if (m_End == m_Size)
			ResizeBuffer(m_ComponentSize * m_Size * 2);
		uint index = m_End;
		memcpy((unsigned char*)(m_Buffer) + index * m_ComponentSize, component, m_ComponentSize);
		m_End++;
		m_Handles.push_back(index);
		return m_Handles.size() - 1;
	}

	void RemoveComponent(uint index) {
		if (m_Handles[index] > m_End) {
			return;
		} else if (m_Handles[index] == m_End) {
			m_End--;
		} else {
			index = m_Handles[index];
			//move end to new position
			memcpy((unsigned char*)m_Buffer + index * m_ComponentSize, (unsigned char*)m_Buffer + m_End * m_ComponentSize, m_ComponentSize);
			m_Handles[m_End] = index; //update handles
			m_End--; //shorten end
		}

	}

	void* GetComponent(uint index) {
		if (index >= m_Size)
			return nullptr;
		uint i = m_Handles[index];
		return (unsigned char*)(m_Buffer) + i * m_ComponentSize;
	}
	
	void* GetComponentList() {
		return m_Buffer;
	}

	uint GetListSize() {
		return m_End;
	}

private:
	uint m_Size = 0;
	uint m_End = 0;
	uint m_ComponentSize = 0;
	void* m_Buffer = nullptr;
	std::vector<uint> m_Handles;
};