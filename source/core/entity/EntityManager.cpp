#include "EntityManager.h"

EntityManager::EntityManager(){}

EntityManager::~EntityManager(){}

EntityManager& EntityManager::GetInstance() {
	static EntityManager instance;
	return instance;
}

Entity& EntityManager::CreateEntity() {
	Entity e;
	e.ComponentBitfield = 0;
	e.Components.reserve(ENTITY_DEFAULT_COMPONENT_COUNT);
	e.UID = m_Counter++;
	m_Entities.push_back(e);
	return m_Entities.back();
}

void EntityManager::RemoveEntity(Entity& entity) {
	//potentially slow
	for (std::vector<Entity>::iterator it = m_Entities.begin(); it != m_Entities.end(); it++) {
		if (it._Ptr->UID = entity.UID) {
			m_Entities.erase(it);
		}
	}
}

std::vector<Entity>& EntityManager::GetEntityList() {
	return m_Entities;
}