#pragma once
#include "ComponentBuffer.h"
#include "../components/ComponentTypes.h"
#include "../entity/Entity.h"
#include <map>
#define g_ComponentManager ComponentManager::GetInstance()
struct LastComponent;
class ComponentManager {
public:
	~ComponentManager();
	static ComponentManager& GetInstance();
	void Init();

	template<typename component>
	void CreateComponent(const component& comp, Entity& ent, const int type ) {
		auto& buffer = m_Buffers.find(type);
		if (buffer != m_Buffers.end()) {
			int index = buffer->second.AddComponent<component>(comp);
			if (ent.Components.size() < m_Buffers.size()) {
				ent.Components.resize(m_Buffers.size());
			}
			ent.ComponentBitfield = ent.ComponentBitfield | CreateBitFlag(buffer->first);
			ent.Components[type] = index;
		} else {
			printf("trying to create component without initializing a buffer\n");
		}
	}
	template<typename component>
	void RemoveComponent(Entity& ent) {
		int typeID = GetComponentTypeID<component>();
		auto& buffer = m_Buffers.find(typeID);
		if (buffer != m_Buffers.end()) {
			buffer->second.RemoveComponent(ent.Components[typeID]);
			ent.Components[typeID] = 0;
			ent.ComponentBitfield ~&= CreateBitFlags(buffer->first); 
		} else {
			printf("trying to remove component without initializing a buffer\n");
		}
	}


	template<typename component>
	int GetBuffer(component** outBuffer) {
		int type = GetComponentTypeID<component>();
		auto buffer = m_Buffers.find(type);

		if (buffer != m_Buffers.end()) {
			
			*outBuffer = (component*)buffer->second.GetComponentList();
			return buffer->second.GetListSize();
		}
		else {
			printf("No componentbuffer of such type: %d\n", GetComponentTypeID<component>());
			outBuffer = nullptr;
			return -1;
		}
	}

	template<typename component>
	component* GetComponent(Entity& ent) {
		int type = GetComponentTypeID<component>();
		auto buffer = m_Buffers.find(type);

		if (buffer != m_Buffers.end()) {
			return buffer->second.GetComponent<component>(ent.Components[type]);
		} else {
			printf("Error getting component\n");
			return nullptr;
		}
	}

private:
	ComponentManager();

	template<typename t>
	int CreateComponentBuffer(int size) {
		ComponentBuffer buffer;
		buffer.CreateBuffer<t>(size);
		int id = GetComponentTypeID<t>();
		m_Buffers[id] = buffer;
		return id;
	}

	std::map<int,ComponentBuffer>	m_Buffers;
	int								m_ComponentTypeCount;
};