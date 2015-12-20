#include "ComponentManager.h"
#include "../components/CameraComponent.h"
#include "../components/PlacementComponent.h"
#include "../components/ModelComponent.h"
#include "../components/RigidBodyComponent.h"
#include "../components/CollisionComponent.h"
#include <cmath>
ComponentManager::ComponentManager() {

}

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
	CreateComponentBuffer(100, sizeof(PlacementComponent), PlacementComponent::Flag);
	CreateComponentBuffer(100, sizeof(ModelComponent), ModelComponent::Flag);
	CreateComponentBuffer(100, sizeof(RigidBodyComponent), RigidBodyComponent::Flag);
	CreateComponentBuffer(100, sizeof(CollisionComponent), CollisionComponent::Flag);
	CreateComponentBuffer(1, sizeof(CameraComponent), CameraComponent::Flag);
} 


void ComponentManager::CreateComponent(const void* comp, Entity& ent, uint type) {
	auto& buffer = m_Buffers.find(type);
	if (buffer != m_Buffers.end()) {
		int index = buffer->second.AddComponent(comp);

		if (ent.Components.size() < m_Buffers.size()) {
			ent.Components.resize(m_Buffers.size());
		}
		ent.ComponentBitfield = ent.ComponentBitfield | type;
		ent.Components[log2(type)] = index;
	}
	else {
		printf("trying to create component without initializing a buffer\n");
	}
}

void ComponentManager::RemoveComponent(Entity& ent, uint type) {
	auto& buffer = m_Buffers.find(type);
	if (buffer != m_Buffers.end()) {
		buffer->second.RemoveComponent(ent.Components[log2(type)]);
		ent.Components[log2(type)] = 0;
		ent.ComponentBitfield &= ~type;
	}
	else {
		printf("trying to remove component without initializing a buffer\n");
	}
}

int ComponentManager::GetBuffer(void** outBuffer, uint type) {
	auto buffer = m_Buffers.find(type);

	if (buffer != m_Buffers.end()) {

		*outBuffer = (void*)buffer->second.GetComponentList();
		return buffer->second.GetListSize();
	}
	else {
		printf("No componentbuffer of such type: %d\n", type);
		*outBuffer = nullptr;
		return -1;
	}
}

void* ComponentManager::GetComponent(Entity& ent, uint type) {
	auto buffer = m_Buffers.find(type);

	if (buffer != m_Buffers.end()) {
		return buffer->second.GetComponent(ent.Components[log2(type)]);
	}
	else {
		printf("Error getting component\n");
		return nullptr;
	}
}

void ComponentManager::CreateComponentBuffer(uint count, uint componentSize, uint id) {
	ComponentBuffer buffer;
	buffer.CreateBuffer(count, componentSize);
	m_Buffers[id] = buffer;
}