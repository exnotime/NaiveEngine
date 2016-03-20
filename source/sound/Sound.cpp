#include "Sound.h"
using namespace sfx;

Sound::Sound() {

}

Sound::Sound( irrklang::ISound* sound ) {
	m_Sound = sound;
}

Sound::~Sound() {
	m_Sound->drop();
}

void Sound::Pause() {
	m_Sound->setIsPaused(true);
}

void Sound::SetVolume( float volume ) {
	m_Sound->setVolume(volume);
}

void Sound::SetPan( float pan ) {
	m_Sound->setPan(pan);
}
void Sound::SetVelocity( const glm::vec3& vel ) {
	m_Sound->setVelocity(irrklang::vec3df(vel.x,vel.y,vel.z));
}
void Sound::SetPosition( const glm::vec3& pos ) {
	m_Sound->setPosition(irrklang::vec3df(pos.x,pos.y,pos.z));
}
void Sound::SetPlaybackSpeed( float speed ) {
	m_Sound->setPlaybackSpeed(speed);
}
void Sound::Skip( float pos ) {
	m_Sound->setPlayPosition(pos);
}