#include "SSGraphics.h"
#include <gfx/GraphicsEngine.h>
#include <gfx/ModelBank.h>
#include <gfx/Camera.h>
#include <glm/gtx/transform.hpp>
#include <gfx/LightEngine.h>
#include "../../datasystem/ComponentManager.h"
#include "../../EntityManager.h"
#include "../../components/PlacementComponent.h"
#include "../../components/ComponentTypes.h"
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
	m_Model = gfx::g_ModelBank.LoadModel("asset/model/cube.dae");
	gfx::g_ModelBank.BuildBuffers();
	Entity& e = g_EntityManager.CreateEntity();
	PlacementComponent pc;
	pc.Position = glm::vec3(0, 0, -2);
	g_ComponentManager.CreateComponent<PlacementComponent>(pc, e);
	{
		int type = GetComponentTypeID<gfx::Camera>();
		int type2 = GetComponentTypeID<gfx::Camera>();
		int type3 = GetComponentTypeID<PlacementComponent>();
	}
	{
		int type4 = GetComponentTypeID<gfx::Camera>();
		int type5 = GetComponentTypeID<gfx::Camera>();
		int type6 = GetComponentTypeID<PlacementComponent>();
	}
}

void SSGraphics::Update(const float deltaTime) {
	gfx::Camera* camera;
	int i = g_ComponentManager.GetBuffer<gfx::Camera>(&camera);
	gfx::View view;
	view.camera = camera->GetData();
	view.viewport.height = 900;
	view.viewport.width = 1600;
	view.viewport.x = 0;
	view.viewport.y = 0;
	m_RenderQueue->AddView(view);
	std::vector<gfx::ShaderInput> inputList;
	gfx::ShaderInput input;
	PlacementComponent* pcs;
	for (int pc = 0; pc < g_ComponentManager.GetBuffer<PlacementComponent>(&pcs); pc++) {
		input.World = glm::translate(pcs[pc].Position) * glm::scale(pcs[pc].Scale) * glm::mat4_cast(pcs[pc].Orientation);
		input.Color = glm::vec4(1);
		inputList.push_back(input);
	}
	m_RenderQueue->Enqueue(m_Model, inputList);

	gfx::Light dl;
	dl.Color = glm::vec4(1);
	dl.Direction = glm::vec3(0, -0.9f, 0.1f);
	gfx::g_LightEngine.AddDirLightToQueue(dl);


	m_GFXEngine->Draw();
	m_RenderQueue->Clear();

}

void SSGraphics::Shutdown() {
	delete m_GFXEngine;
}

