#pragma once
#include "SoundLibraryDefine.h"
#include <irrKlang/irrKlang.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Sound.h"
namespace sfx {
	 struct SOUND_API SoundData {
		glm::vec3 Position;
		float MinDistance;
		float MaxDistance;
		bool Loop;
	};

#define g_SoundEngine SoundEngine::GetInstance()
	class SOUND_API SoundEngine{
	public:
		static SoundEngine& GetInstance();
		~SoundEngine();

		void Init();
		void UpdateListener(const glm::vec3& listenerPos, const glm::vec3& forwardVec, const glm::vec3& velocity = glm::vec3(0));
		void Shutdown();

		Sound* PlaySound(const std::string& file,const SoundData& data);

	private:
		SoundEngine();
		irrklang::ISoundEngine* m_SoundEngine;
		std::vector<Sound*> m_Sounds;
	};
}