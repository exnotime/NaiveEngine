#include "SSGraphics.h"
#include <gfx/GraphicsEngine.h>
#include <gfx/ModelBank.h>
#include <gfx/Camera.h>
#include <glm/gtx/transform.hpp>
#include <gfx/LightEngine.h>
#include "../../datasystem/ComponentManager.h"
#include "../../entity/EntityManager.h"
#include "../../components/PlacementComponent.h"
#include "../../components/ModelComponent.h"
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
	m_Model = gfx::g_ModelBank.LoadModel("asset/model/cube.obj");
	
}

void SSGraphics::Update(const float deltaTime) {
	gfx::g_ModelBank.BuildBuffers();
	gfx::Camera* camera;
	g_ComponentManager.GetBuffer<gfx::Camera>(&camera);
	gfx::View view;
	view.camera = camera->GetData();
	view.viewport.height = 900;
	view.viewport.width = 1600;
	view.viewport.x = 0;
	view.viewport.y = 0;
	m_RenderQueue->AddView(view);

	std::vector<gfx::ShaderInput> inputList;
	gfx::ShaderInput input;
	int flag = PlacementComponent::Flag | ModelComponent::Flag;

	for (auto& entity : g_EntityManager.GetEntityList()){
		if (entity.ComponentBitfield & flag) {
			PlacementComponent* pc = g_ComponentManager.GetComponent<PlacementComponent>(entity);
			ModelComponent* mc = g_ComponentManager.GetComponent<ModelComponent>(entity);

			input.World = glm::translate(pc->Position) * glm::scale(pc->Scale) * glm::mat4_cast(pc->Orientation);
			input.Color = mc->Color;
			m_RenderQueue->Enqueue(mc->Model, input);
		}
	}

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

