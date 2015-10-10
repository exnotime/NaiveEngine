#pragma once
#include "Entity.h"
#include <vector>
#define g_EntityManager EntityManager::GetInstance()
class EntityManager {
public:
	~EntityManager();
	static EntityManager& GetInstance();

	void CreateEntity(const Entity& entity);
	void RemoveEntity(Entity& entity);

	std::vector<Entity>& GetEntityList();
private:
	EntityManager();
	std::vector<Entity> m_Entities;
};