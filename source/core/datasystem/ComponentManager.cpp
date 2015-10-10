#include "ComponentManager.h"
#include "../components/PlacementComponent.h"
#include "../components/ModelComponent.h"
ComponentManager::~ComponentManager(){
	//release all buffers
	for (auto& buffer : m_Buffers) {
		buffer.second.DestroyBuffer();
	}
}

ComponentManager& ComponentManager::GetInstance(){
	static ComponentManager instance;
	return instance;
}

void ComponentManager::Init(){
	//create Buffers for every component in the game
	CreateComponentBuffer<PlacementComponent>(1000);
	CreateComponentBuffer<ModelComponent>(1000);
} 

ComponentManager::ComponentManager() {
}