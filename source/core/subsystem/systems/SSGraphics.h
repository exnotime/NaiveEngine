#pragma once
#include "../SubSystem.h"
namespace gfx {
	class GraphicsEngine;
	class RenderQueue;
}
class SSGraphics : public SubSystem {
public:
	SSGraphics();
	~SSGraphics();

	virtual void Startup();
	virtual void Update(const float deltaTime);
	virtual void Shutdown();
	
private:
	gfx::GraphicsEngine* m_GFXEngine;
	gfx::RenderQueue* m_RenderQueue;
	int m_Model;
};