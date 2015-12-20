#include "SSCamera.h"
#include "../../datasystem/ComponentManager.h"
#include "../../entity/EntityManager.h"
#include "../../components/CameraComponent.h"
#include <imgui/imgui.h>
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
	CameraComponent cam;
	g_ComponentManager.CreateComponent(&cam, entity, CameraComponent::Flag);
}

void SSCamera::Update(const float deltaTime) {
	CameraComponent* camera;
	for (int comp = 0; comp < g_ComponentManager.GetBuffer((void**)&camera, CameraComponent::Flag); comp++) {
		camera->Camera.PitchRelative(g_Input.GetMouseDelta().y * -0.05f * deltaTime);
		camera->Camera.YawWorld(g_Input.GetMouseDelta().x * -0.05f * deltaTime);

		if (g_Input.IsKeyDown(GLFW_KEY_W)) {
			camera->Camera.MoveRelative(glm::vec3(0, 0, -1) * deltaTime * MOVE_SPEED);
		}
		if (g_Input.IsKeyDown(GLFW_KEY_S)) {
			camera->Camera.MoveRelative(glm::vec3(0, 0, 1) * deltaTime * MOVE_SPEED);
		}
		if (g_Input.IsKeyDown(GLFW_KEY_A)) {
			camera->Camera.MoveRelative(glm::vec3(-1, 0, 0) * deltaTime * MOVE_SPEED);
		}
		if (g_Input.IsKeyDown(GLFW_KEY_D)) {
			camera->Camera.MoveRelative(glm::vec3(1, 0, 0) * deltaTime * MOVE_SPEED);
		}

		camera->Camera.CalculateViewProjection();
	}
}

void SSCamera::Shutdown() {

}