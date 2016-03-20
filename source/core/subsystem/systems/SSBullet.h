#pragma once
#include "../SubSystem.h"
class SSBullet : public SubSystem {
public:
	SSBullet();
	~SSBullet();

	virtual void Startup();
	virtual void Update(const float deltaTime);
	virtual void Shutdown();
private:

};

