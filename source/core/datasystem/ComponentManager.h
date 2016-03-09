#pragma once
#include "ComponentBuffer.h"
#include "../entity/Entity.h"
#include <map>
#define g_ComponentManager ComponentManager::GetInstance()
class ComponentManager {
public:
	~ComponentManager();
	static ComponentManager& GetInstance();
	void Init();

	void CreateComponent(const void* comp, Entity& ent, uint type);
	void RemoveComponent(Entity& ent, uint type);
	int GetBuffer(void** outBuffer, uint type);
	void* GetComponent(Entity& ent, uint type);
	void PrintInfo();
private:
	ComponentManager();
	void CreateComponentBuffer(uint count, uint componentSize, uint id);

	std::map<unsigned int,ComponentBuffer>	m_Buffers;
	int								m_ComponentTypeCount;
};