#include "EntityManager.h"

EntityManager::EntityManager(){}

EntityManager::~EntityManager(){}

EntityManager& EntityManager::GetInstance() {
	static EntityManager instance;
	return instance;
}