#include "SSCamera.h"
#include "../../datasystem/ComponentManager.h"
#include "../../EntityManager.h"
#include <imgui/imgui.h>
#include <gfx/Camera.h>
SSCamera::SSCamera() {

}

SSCamera::~SSCamera() {

}

void SSCamera::Startup() {
	//create Camera Entity
	m_CameraEntity = &g_EntityManager.CreateEntity();
	//create a camera component
	gfx::Camera cam;
	g_ComponentManager.CreateComponent<gfx::Camera>(cam,*m_CameraEntity);
}

void SSCamera::Update(const float deltaTime) {
	ImGuiIO io = ImGui::GetIO();
	gfx::Camera* camera;
	for (int comp = 0; comp < g_ComponentManager.GetBuffer<gfx::Camera>(&camera); comp++) {
		if (io.KeysDown[ImGuiKey_A]) {
			camera->MoveRelative(glm::vec3(-1, 0, 0));
			camera->CalculateViewProjection();
		}
	}
}

void SSCamera::Shutdown() {

}