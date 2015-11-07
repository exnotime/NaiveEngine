#include "engine.h"
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include "datasystem/ComponentManager.h"
#include "subsystem/systems/SSCamera.h"
#include "subsystem/systems/SSGraphics.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_sdl.h"
#include <GL/glew.h>
using namespace core;
Engine::Engine() {

}

Engine::~Engine() {
	delete m_Window;
	m_SubSystemSet.ShutdownSubSystems();
	ImGui_ImplSdl_Shutdown();
}

void Engine::Init() {
	//set up window
	m_Window = new Window();
	WindowSettings ws;
	ws.Width = 1600;
	ws.Height = 900;
	ws.HighDPI = true;
	ws.OpenGL = true;
	ws.Title = "Naive engine";
	ws.Vsync = true;
	m_Window->Initialize(ws);
	ImGui_ImplSdl_Init(m_Window->GetWindow());
	g_ComponentManager.Init();

	SSCamera* cameraSubsys = new SSCamera();
	SSGraphics* gfxSubSys = new SSGraphics();
	m_SubSystemSet.AddSubSystem(cameraSubsys, 0, 0, 0);
	m_SubSystemSet.AddSubSystem(gfxSubSys, 0, 1, 0);
	m_SubSystemSet.StartSubSystems();
}

void Engine::Run() {
	//handle events
	SDL_Event event;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSdl_ProcessEvent(&event);
			switch (event.type) {
			case SDL_QUIT:
					return;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
					return;
				break;
			}
		}
		ImGui_ImplSdl_NewFrame(m_Window->GetWindow());
		m_SubSystemSet.UpdateSubSystems(ImGui::GetIO().DeltaTime);

		//render imgui
		ImGui::ShowMetricsWindow();
		glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
		ImGui::Render();
		SDL_GL_SwapWindow(m_Window->GetWindow());
	}
}