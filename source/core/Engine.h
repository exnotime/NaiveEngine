#pragma once
#include "Window.h"
#include "subsystem/SubSystemSet.h"
//directx 12 stuff
//#include <gfx_dx/DXEngine.h>
namespace core {
	class Engine {
	public:
		Engine();
		~Engine();
		void Init();
		void Run();
	private:
		Window* m_Window;
		//gfx_dx::DXengine* m_DXEngine;
		SubSystemSet m_SubSystemSet;
	};
}