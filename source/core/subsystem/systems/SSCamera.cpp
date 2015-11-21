#include "SSCamera.h"
#include "../../datasystem/ComponentManager.h"
#include "../../entity/EntityManager.h"
#include <imgui/imgui.h>
#include <gfx/Camera.h>
#include "../../Input/Input.h"
#define MOVE_SPEED 10.0f
SSCamera::SSCamera() {

}

SSCamera::~SSCamera() {

}

void SSCamera::Startup() {
	//create Camera Entity
	Entity& entity = g_EntityManager.CreateEntity();
	//create a camera component
	gfx::Camera cam;
	g_ComponentManager.CreateComponent<gfx::Camera>(cam, entity, GET_TYPE_ID(gfx::Camera));
}

void SSCamera::Update(const float deltaTime) {
	gfx::Camera* camera;
	for (int comp = 0; comp < g_ComponentManager.GetBuffer<gfx::Camera>(&camera); comp++) {
		camera->PitchRelative(g_Input.GetMouseDelta().y * -0.05f * deltaTime);
		camera->YawWorld(g_Input.GetMouseDelta().x * -0.05f * deltaTime);

		if (g_Input.IsKeyDown(GLFW_KEY_W)) {
			camera->MoveRelative(glm::vec3(0, 0, -1) * deltaTime * MOVE_SPEED);
		}
		if (g_Input.IsKeyDown(GLFW_KEY_S)) {
			camera->MoveRelative(glm::vec3(0, 0, 1) * deltaTime * MOVE_SPEED);
		}
		if (g_Input.IsKeyDown(GLFW_KEY_A)) {
			camera->MoveRelative(glm::vec3(-1, 0, 0) * deltaTime * MOVE_SPEED);
		}
		if (g_Input.IsKeyDown(GLFW_KEY_D)) {
			camera->MoveRelative(glm::vec3(1, 0, 0) * deltaTime * MOVE_SPEED);
		}

		camera->CalculateViewProjection();
	}
}

void SSCamera::Shutdown() {

}