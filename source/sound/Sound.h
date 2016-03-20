#pragma once
#include "SoundLibraryDefine.h"
#include <irrKlang/irrKlang.h>
#include <glm/glm.hpp>
namespace sfx {
	class SOUND_API Sound {
	public:
		Sound();
		Sound(irrklang::ISound* sound);
		~Sound();

		void Pause();
		void SetVolume(float volume);
		void SetPan(float pan);
		void SetVelocity(const glm::vec3& vel);
		void SetPosition(const glm::vec3& pos);
		void SetPlaybackSpeed(float speed);
		void Skip(float pos);
		
	private:
		irrklang::ISound* m_Sound;
	};
}
