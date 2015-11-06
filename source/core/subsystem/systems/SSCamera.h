#pragma once
#include "../SubSystem.h"
#include "../../Entity.h"
namespace gfx {
	class Camera;
}
class SSCamera : public SubSystem {
public:
	SSCamera();
	~SSCamera();

	virtual void Startup();
	virtual void Update(const float deltaTime);
	virtual void Shutdown();

private:
	Entity* m_CameraEntity;
};