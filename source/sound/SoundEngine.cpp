#include "SoundEngine.h"
#include <stdio.h>
using namespace sfx;

SoundEngine::SoundEngine(){
	
}

SoundEngine& SoundEngine::GetInstance() {
	static SoundEngine instance;
	return instance;
}

SoundEngine::~SoundEngine(){
	Shutdown();
}

void SoundEngine::Init() {
	m_SoundEngine = irrklang::createIrrKlangDevice();
	if( !m_SoundEngine ) {
		printf("Error: could not create a sound engine\n");
	}
}

void SoundEngine::UpdateListener(const glm::vec3& listenerPos, const glm::vec3& forwardVec, const glm::vec3& velocity) {
	m_SoundEngine->setListenerPosition(irrklang::vec3df(listenerPos.x, listenerPos.y, listenerPos.z),
		irrklang::vec3df(forwardVec.x, forwardVec.y, forwardVec.z),
		irrklang::vec3df(velocity.x, velocity.y, velocity.z));
}

Sound* SoundEngine::PlaySound( const std::string& file, const SoundData& data ) {
	irrklang::ISound* iSound = m_SoundEngine->play3D(file.c_str(), irrklang::vec3df( data.Position.x,data.Position.y,data.Position.z), data.Loop, false, true);
	iSound->setMinDistance(data.MinDistance);
	iSound->setMaxDistance(data.MaxDistance);
	Sound* sound = new Sound(iSound);
	m_Sounds.push_back(sound);
	return sound;
}

void SoundEngine::Shutdown() {
	m_Sounds.clear();
	m_SoundEngine->drop();
}