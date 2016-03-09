#include "SSCamera.h"
#include "../../datasystem/ComponentManager.h"
#include "../../entity/EntityManager.h"
#include "../../components/CameraComponent.h"
#include "../../components/PlacementComponent.h"
#include "../../components/RigidBodyComponent.h"
#include "../../Input/Input.h"
#include "../../entity/EntityFactory.h"
#define MOVE_SPEED 500.0f
#define TURN_SPEED 0.0001f * 10.0f
SSCamera::SSCamera() {

}

SSCamera::~SSCamera() {

}

void SSCamera::Startup() {
	
}

void SSCamera::Update(const float deltaTime) {
	int flag = CameraComponent::Flag | PlacementComponent::Flag | RigidBodyComponent::Flag;
	gfx::CameraData cd;
	for (auto& entity : g_EntityManager.GetEntityList()) {
		if ((entity.ComponentBitfield & flag) == flag) {
			PlacementComponent* pc = (PlacementComponent*)g_ComponentManager.GetComponent(entity, PlacementComponent::Flag);
			CameraComponent* cc = (CameraComponent*)g_ComponentManager.GetComponent(entity, CameraComponent::Flag);
			RigidBodyComponent* rbc = (RigidBodyComponent*)g_ComponentManager.GetComponent(entity, RigidBodyComponent::Flag);
			//update camera from the placement component
			cc->Camera.SetPosition(pc->Position);
			//update camera orientation
			cc->Camera.PitchRelative(g_Input.GetMouseDelta().y * TURN_SPEED * -1);
			cc->Camera.YawWorld(g_Input.GetMouseDelta().x * TURN_SPEED * -1);

			cc->Camera.CalculateViewProjection();
			//set velocity
			glm::vec3 velocity = glm::vec3(0);
			if (g_Input.IsKeyDown(GLFW_KEY_W)) {
				glm::vec3 f = cc->Camera.GetForward();
				f.y = 0;
				f = glm::normalize(f);
				velocity += f * deltaTime * MOVE_SPEED;
			}
			if (g_Input.IsKeyDown(GLFW_KEY_S)) {
				glm::vec3 f = cc->Camera.GetForward();
				f.y = 0;
				f = glm::normalize(f);
				velocity += f * deltaTime * MOVE_SPEED * -1.0f;
			}
			if (g_Input.IsKeyDown(GLFW_KEY_A)) {
				glm::vec3 f = cc->Camera.GetRight();
				f.y = 0;
				f = glm::normalize(f);
				velocity += f * deltaTime * MOVE_SPEED * -1.0f;
			}
			if (g_Input.IsKeyDown(GLFW_KEY_D)) {
				glm::vec3 f = cc->Camera.GetRight();
				f.y = 0;
				f = glm::normalize(f);
				velocity += f * deltaTime * MOVE_SPEED;
			}
			if (g_Input.IsKeyDown(GLFW_KEY_SPACE)) {
				velocity.y = 50 * deltaTime;
			}
			//update placement
			pc->World = cc->Camera.GetView();
			pc->Orientation = glm::quat_cast(pc->World);
			pc->Position = glm::vec3(pc->World[3]);
			//update rigid body
			rbc->Body->setLinearVelocity(btVector3(velocity.x, rbc->Body->getLinearVelocity().y() + velocity.y, velocity.z));

			cd = cc->Camera.GetData();
		}
	}
	if (g_Input.IsMousebuttonDown(GLFW_MOUSE_BUTTON_1)) {
		SpawnBullet(cd.Position + cd.Forward * 2.0f + cd.Right * -0.05f, cd.Forward, 1);
	}
}

void SSCamera::Shutdown() {

}