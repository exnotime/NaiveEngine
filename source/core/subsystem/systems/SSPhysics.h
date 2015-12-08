#pragma once
#include "../SubSystem.h"
class SSPhysics : public SubSystem {
public:
	SSPhysics();
	~SSPhysics();

	virtual void Startup();
	virtual void Update(const float deltaTime);
	virtual void Shutdown();

private:
}; 