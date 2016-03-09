#include "engine.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include "entity/EntityManager.h"
#include "datasystem/ComponentManager.h"
#include "subsystem/systems/SSCamera.h"
#include "subsystem/systems/SSGraphics.h"
#include "subsystem/systems/SSStartup.h"
#include "subsystem/systems/SSPhysics.h"
#include "Input/Input.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw_gl3.h"
#include <time.h>
#include <utility/Randomizer.h>
#include "Timer.h"
//#include <gfx_dx/DXEngine.h>

using namespace core;
Engine::Engine() {

}

Engine::~Engine() {
	delete m_Window;
	m_SubSystemSet.ShutdownSubSystems();
	ImGui_ImplGlfwGL3_Shutdown();
	//delete m_DXEngine;
	glfwTerminate();
}

void Engine::Init() {
	g_Randomizer.Seed(time(0));
	//set up window
	m_Window = new Window();
	WindowSettings ws;
	ws.X = 500;
	ws.Y = 250;
	ws.Width = 1600;
	ws.Height = 900;
	ws.HighDPI = true;
	ws.OpenGL = true;
	ws.Title = "NaiveEngine";
	ws.Vsync = true;
	ws.BorderLess = false;
	m_Window->Initialize(ws);
	ImGui_ImplGlfwGL3_Init(m_Window->GetWindow(), true);
	//m_DXEngine = new gfx_dx::DXengine();
	//m_DXEngine->Init(glfwGetWin32Window(m_Window->GetWindow()), 1600, 900);

	g_ComponentManager.Init();
	glfwSetKeyCallback(m_Window->GetWindow(), KeyboardCallBack);
	glfwSetMouseButtonCallback(m_Window->GetWindow(), MouseButtonCallback);
	glfwSetCursorPosCallback(m_Window->GetWindow(), MousePosCallback);
	g_Input.SetCursorMode(m_Window->GetWindow(), GLFW_CURSOR_DISABLED);

	m_SubSystemSet.AddSubSystem(new SSPhysics(), 0, 0, 0);
	m_SubSystemSet.AddSubSystem(new SSCamera(), 0, 1, 0);
	m_SubSystemSet.AddSubSystem(new SSGraphics(), 0, 2, 0);
	m_SubSystemSet.AddSubSystem(new SSStartup(), 0, 3, 0);
	m_SubSystemSet.StartSubSystems();
}

void Engine::Run() {
	int mode = GLFW_CURSOR_DISABLED;
	Timer gameTime;
	while (!glfwWindowShouldClose(m_Window->GetWindow())) {
		if (g_Input.IsKeyPushed(GLFW_KEY_L)) {
			mode = (mode == GLFW_CURSOR_NORMAL) ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
			g_Input.SetCursorMode(m_Window->GetWindow(), mode);
		}

		ImGui_ImplGlfwGL3_NewFrame();

		m_SubSystemSet.UpdateSubSystems(gameTime.Tick());
		//render imgui
		ImGui::ShowMetricsWindow();
		g_EntityManager.PrintInfo();
		g_ComponentManager.PrintInfo();

		glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
		ImGui::Render();

		glfwSwapBuffers(m_Window->GetWindow());

		if (g_Input.IsKeyDown(GLFW_KEY_ESCAPE))
			break;

		g_Input.Update();
		glfwPollEvents();
	}

}