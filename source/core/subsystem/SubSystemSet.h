#pragma once 
#include "SubSystem.h"
#include <stdint.h>
#include <vector>
#define SUBSYSTEM_INPUT_ORDER -1
namespace core {
	struct SubSystemEntry {
		uint32_t start;
		uint32_t update;
		uint32_t shutdown;
		SubSystem* ss;
	};
	class SubSystemSet {
	public:
		SubSystemSet();
		~SubSystemSet();

		void AddSubSystem( SubSystem* ss, uint32_t startUpPrio = 0, uint32_t updatePrio = 0, uint32_t shutdownPrio = 0);
		void StartSubSystems();
		void UpdateSubSystems(const float deltaTime);
		void ShutdownSubSystems();
	private:
		bool m_Updated = false;
		std::vector<SubSystemEntry> m_Entries;
	};
}