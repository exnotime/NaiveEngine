#include "ComponentManager.h"
#include "../components/PlacementComponent.h"
#include "../components/ModelComponent.h"
#include <gfx/Camera.h>

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
	//create Buffers for components in the game
	//remember to set flag for components
	PlacementComponent::Flag = CreateBitFlag(CreateComponentBuffer<PlacementComponent>(100));
	ModelComponent::Flag = CreateBitFlag(CreateComponentBuffer<ModelComponent>(100));
	//no need to set flag for pure data storage
	CreateComponentBuffer<gfx::Camera>(1);

	m_ComponentTypeCount = GetComponentTypeID<LastComponent>();
} 

ComponentManager::ComponentManager() {
}