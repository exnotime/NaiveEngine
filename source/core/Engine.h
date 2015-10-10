#pragma once
#include <gfx/GraphicsEngine.h>
#include <glm/glm.hpp>
#include "Window.h"
#include "Entity.h"
namespace core {
	class Engine {
	public:
		Engine();
		~Engine();
		void Init();
		void Run();
	private:
		gfx::GraphicsEngine* m_GFXEngine;
		gfx::RenderQueue* m_RenderQueue;
		Window* m_Window;
		glm::mat4 m_Proj;
		unsigned int m_Cube;
		Entity m_Entity;
	};
}