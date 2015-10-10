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

	template<typename component>
	void CreateBuffer(uint size) {
		m_Buffer = (Byte*)malloc(sizeof(component) * size);
		assert(m_Buffer != nullptr);
		m_Size = size;
	}
	void ResizeBuffer(uint newSize) {
		m_Buffer = (Byte*)realloc(m_Buffer, newSize);
		assert(m_Buffer != nullptr);
		m_Size *= 2;
	}

	void DestroyBuffer() {
		if (m_Buffer) free(m_Buffer);
	}

	template<typename component>
	uint AddComponent(const component& comp) {
		if (m_End == m_Size)
			ResizeBuffer(sizeof(component) * m_Size * 2);

		uint index = m_End;
		static_cast<component*>(m_Buffer)[index] = comp;
		m_End++;
		m_Handles.push_back(index);

		return m_Handles.size() - 1;
	}
	template<typename component>
	void RemoveComponent(uint index) {
		if (m_Handles[index] > m_End) {
			return;
		} else if (m_Handles[index] == m_End) {
			m_End--;
		} else {
			index = m_Handles[index];
			(component*)(m_Buffer)[index] = (component*)(m_Buffer)[m_End]; //move end to new position
			m_Handles[m_End] = index;
			m_End--;
		}

	}

	template <typename component>
	component* GetComponent(int index) {
		if (index >= m_Size)
			return nullptr;
		return &((component*)(m_Buffer))[m_Handles[index]];
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
	void* m_Buffer = nullptr;
	std::vector<uint> m_Handles;
};