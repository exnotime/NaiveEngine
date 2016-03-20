#pragma once
#include "../SubSystem.h"
#include <Sound/SoundEngine.h>
class SSMusic : public SubSystem {
public:
	SSMusic();
	~SSMusic();

	virtual void Startup();
	virtual void Update(const float deltaTime);
	virtual void Shutdown();
private:

};

