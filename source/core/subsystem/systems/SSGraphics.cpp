#include "SSGraphics.h"
#include <gfx/GraphicsEngine.h>
#include <gfx/ModelBank.h>
#include <gfx/Camera.h>
#include "../../datasystem/ComponentManager.h"
#include "../../components/PlacementComponent.h"
SSGraphics::SSGraphics() {

}

SSGraphics::~SSGraphics() {

}

void SSGraphics::Startup() {
	m_GFXEngine = new gfx::GraphicsEngine();
	gfx::GraphicsSettings gs;
	gs.Width = 1600;
	gs.Height = 900;
	m_GFXEngine->Initialize(gs);

	m_RenderQueue = m_GFXEngine->GetRenderQueue();
}

void SSGraphics::Update(const float deltaTime) {
	gfx::Camera* camera;
	g_ComponentManager.GetBuffer<gfx::Camera>(&camera);
	gfx::View view;
	view.camera = camera->GetData();
	view.viewport.height = 900;
	view.viewport.width = 1600;
	view.viewport.x = 0;
	view.viewport.y = 0;
	m_RenderQueue->AddView(view);
	PlacementComponent* pcs;
	for (int pc = 0; pc < g_ComponentManager.GetBuffer<PlacementComponent>(&pcs); pc++) {

	}

}

void SSGraphics::Shutdown() {

}

