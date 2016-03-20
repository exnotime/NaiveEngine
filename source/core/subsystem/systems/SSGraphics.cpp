#include "SSGraphics.h"
#include <gfx/GraphicsEngine.h>
#include <gfx/ModelBank.h>
#include <glm/gtx/transform.hpp>
#include <gfx/LightEngine.h>
#include "../../datasystem/ComponentManager.h"
#include "../../entity/EntityManager.h"
#include "../../components/PlacementComponent.h"
#include "../../components/ModelComponent.h"
#include "../../components/CameraComponent.h"
#include "../../components/LightComponent.h"
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
	CameraComponent* camera;
	g_ComponentManager.GetBuffer((void**)&camera, CameraComponent::Flag);
	gfx::View view;
	view.camera = camera->Camera.GetData();
	view.camera.Forward = camera->Camera.GetForward();
	view.camera.Up = camera->Camera.GetUp();
	view.camera.Right = camera->Camera.GetRight();
	view.viewport.height = 900;
	view.viewport.width = 1600;
	view.viewport.x = 0;
	view.viewport.y = 0;
	m_RenderQueue->AddView(view);

	std::vector<gfx::ShaderInput> inputList;
	gfx::ShaderInput input;
	int flag = PlacementComponent::Flag | ModelComponent::Flag;

	for (auto& entity : g_EntityManager.GetEntityList()){
		if ((entity.ComponentBitfield & flag) == flag) {
			PlacementComponent* pc = (PlacementComponent*)g_ComponentManager.GetComponent(entity, PlacementComponent::Flag);
			ModelComponent* mc = (ModelComponent*)g_ComponentManager.GetComponent(entity, ModelComponent::Flag);

			input.World = glm::translate(pc->Position) * glm::scale(pc->Scale) * glm::mat4_cast(pc->Orientation);
			input.Color = mc->Color;
			m_RenderQueue->Enqueue(mc->Model, input);
		}
	}

	gfx::Light dl;
	dl.Color = glm::vec4(0.5f);
	dl.Direction = glm::vec3(0, -0.9f, 0.1f);
	gfx::g_LightEngine.AddDirLightToQueue(dl);

	flag = LightComponent::Flag | PlacementComponent::Flag;

	for (auto& entity : g_EntityManager.GetEntityList()){
		if ((entity.ComponentBitfield & flag) == flag) {
			//TODO: make a switch for different lights
			LightComponent* lc = (LightComponent*)g_ComponentManager.GetComponent(entity, LightComponent::Flag);
			PlacementComponent* pc = (PlacementComponent*)g_ComponentManager.GetComponent(entity, PlacementComponent::Flag);
			lc->Light.Position = pc->Position;
			gfx::g_LightEngine.AddPointLightToQueue(lc->Light);
		}
	}

	m_GFXEngine->Draw();
	m_RenderQueue->Clear();

}

void SSGraphics::Shutdown() {
	delete m_GFXEngine;
}

