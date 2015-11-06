#include "engine.h"
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <gfx/ModelBank.h>
#include <gfx/RenderQueue.h>
#include <gfx/LightEngine.h>
#include "components/PlacementComponent.h"
#include "datasystem/ComponentManager.h"
#include <imgui/imgui.h>
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_sdl.h"
using namespace core;
Engine::Engine() {

}

Engine::~Engine() {
	delete m_Window;
	delete m_GFXEngine;
	ImGui_ImplSdl_Shutdown();
}

void Engine::Init() {
	//set up window
	m_Window = new Window();
	WindowSettings ws;
	ws.Width = 1280;
	ws.Height = 720;
	ws.HighDPI = true;
	ws.OpenGL = true;
	ws.Title = "Naive engine";
	ws.Vsync = true;
	m_Window->Initialize(ws);
	ImGui_ImplSdl_Init(m_Window->GetWindow());
	g_ComponentManager.Init();
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

		gfx::ShaderInput input;
		PlacementComponent* pcs = nullptr;
		g_ComponentManager.GetBuffer<PlacementComponent>(&pcs);
		for (int i = 0; i < g_ComponentManager.GetBuffer<PlacementComponent>(&pcs); i++) {
			input.World = glm::translate(pcs[i].Position) * glm::scale(pcs[i].Scale) * glm::mat4_cast(pcs[i].Orientation);
			input.Color = glm::vec4(1, 1, 1, 1);
			m_RenderQueue->Enqueue(m_Cube, input);
		}
		m_RenderQueue->AddView(view);

		gfx::Light dirLight;
		dirLight.Color = glm::vec4(1.0f);
		dirLight.Direction = glm::vec3(0.4f, -1.0f, 0.4f);
		gfx::g_LightEngine.AddDirLightToQueue(dirLight);
		

		m_GFXEngine->Draw();
		//render imgui
		ImGui::ShowMetricsWindow();
		glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
		ImGui::Render();
		SDL_GL_SwapWindow(m_Window->GetWindow());
		m_RenderQueue->Clear();
	}
}