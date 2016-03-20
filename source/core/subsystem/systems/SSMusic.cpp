#include "SSMusic.h"
#include "../../datasystem/ComponentManager.h"
#include "../../components/CameraComponent.h"
SSMusic::SSMusic(){

}

SSMusic::~SSMusic(){

}

void SSMusic::Startup() {
	sfx::g_SoundEngine.Init();

	sfx::SoundData d;
	d.MinDistance = 0;
	d.Loop = true;
	sfx::g_SoundEngine.PlaySound("asset/sound/lilac_4b.mp3",d);
	d.MinDistance = 0;
	d.MaxDistance = 1.5f;
	d.Position = glm::vec3(10,-10,0);
	sfx::g_SoundEngine.PlaySound("asset/sound/fireplace.wav",d);
}

void SSMusic::Update(const float deltaTime) {
	CameraComponent* cam;
	g_ComponentManager.GetBuffer((void**)&cam, CameraComponent::Flag);
	sfx::g_SoundEngine.UpdateListener(cam->Camera.GetPosition(), cam->Camera.GetForward());
}

void SSMusic::Shutdown() {

}

