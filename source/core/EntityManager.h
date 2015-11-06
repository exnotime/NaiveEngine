#pragma once
#include "Entity.h"
#include <vector>
#define ENTITY_DEFAULT_COMPONENT_COUNT 10
#define g_EntityManager EntityManager::GetInstance()
class EntityManager {
public:
	~EntityManager();
	static EntityManager& GetInstance();

	Entity& CreateEntity();
	void RemoveEntity(Entity& entity);

	std::vector<Entity>& GetEntityList();
private:
	EntityManager();
	std::vector<Entity> m_Entities;
	uint64_t m_Counter = 0;
};