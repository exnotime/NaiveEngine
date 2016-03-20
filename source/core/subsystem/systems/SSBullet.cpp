#include "SSBullet.h"
#include "../../Input/Input.h"
#include "../../entity/EntityFactory.h"
#include "../../datasystem/ComponentManager.h"
#include "../../components/CameraComponent.h"
#include "../../components/PlacementComponent.h"
#include <Physics/PhysicsEngine.h>
#include <Sound/SoundEngine.h>
#include "../../entity/EntityManager.h"

SSBullet::SSBullet(){

}

SSBullet::~SSBullet(){

}

void SSBullet::Startup() {
}

void SSBullet::Update(const float deltaTime) {

	CameraComponent* cam;
	g_ComponentManager.GetBuffer((void**)&cam, CameraComponent::Flag);
	if (g_Input.IsMousebuttonPushed(GLFW_MOUSE_BUTTON_1)) {
		glm::vec3 position = cam->Camera.GetPosition() + cam->Camera.GetForward() * 5.0f + cam->Camera.GetRight() * 1.0f + cam->Camera.GetUp() * -0.7f;
		SpawnBullet(position, cam->Camera.GetForward(), 1000);

		sfx::SoundData sd;
		sd.Position = position;
		sd.Loop = false;
		sd.MinDistance = 10.0f;
		sfx::g_SoundEngine.PlaySound("asset/sound/fireball.wav",sd)->SetPlaybackSpeed(3.0f);
	}
	//remove dead bullets
	for (int i = 0; i < g_EntityManager.GetEntityList().size(); ++i) {
		Entity& e = g_EntityManager.GetEntityList().at(i);
		if (e.Tag == ENTITY_TYPE::BULLET) {
			PlacementComponent* pc = (PlacementComponent*)g_ComponentManager.GetComponent(e, PlacementComponent::Flag);
			//remove bullets that falls into the abyss
			if (pc->Position.y < -100) {
				g_PhysicsEngine.RemovePhysicsObjectFromEntity(e.UID);
				g_EntityManager.RemoveEntity(e);
				i--;
			}
		}
	}
}

void SSBullet::Shutdown() {
}

