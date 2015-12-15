#include "SSGraphics.h"
#include <gfx/GraphicsEngine.h>
#include <gfx/ModelBank.h>
#include <gfx/Camera.h>
#include <glm/gtx/transform.hpp>
#include <gfx/LightEngine.h>
#include <imgui/imgui.h>
#include "../../Input/Input.h"
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
}

void SSGraphics::Update(const float deltaTime) {
	gfx::g_ModelBank.BuildBuffers();
	gfx::Camera* camera;
	g_ComponentManager.GetBuffer<gfx::Camera>(&camera);
	gfx::View view;
	view.camera = camera->GetData();
	view.camera.Right = camera->GetRight();
	view.camera.Up = camera->GetUp();
	view.viewport.height = 900;
	view.viewport.width = 1600;
	view.viewport.x = 0;
	view.viewport.y = 0;
	m_RenderQueue->AddView(view);

	std::vector<gfx::ShaderInput> inputList;
	gfx::ShaderInput input;
	int flag = PlacementComponent::Flag | ModelComponent::Flag;

	for (auto& entity : g_EntityManager.GetEntityList()){
		if ((flag & entity.ComponentBitfield) == flag) {
			PlacementComponent* pc = g_ComponentManager.GetComponent<PlacementComponent>(entity);
			ModelComponent* mc = g_ComponentManager.GetComponent<ModelComponent>(entity);

			input.World = glm::translate(pc->Position) * glm::scale(pc->Scale) * glm::mat4_cast(pc->Orientation);
			input.Color = mc->Color;
			m_RenderQueue->Enqueue(mc->Model, input, mc->Color.a);
		}
	}

	gfx::Light dl;
	dl.Color = glm::vec4(1);
	dl.Direction = glm::vec3(0, -0.9f, 0.1f);
	gfx::g_LightEngine.AddDirLightToQueue(dl);
	gfx::Light pl;
	pl.Color = glm::vec4(0,1,0,1);
	pl.Intensity = 1.0f;
	pl.Position = camera->GetPosition();
	pl.Range = 1000.0f;
	gfx::g_LightEngine.AddPointLightToQueue(pl);

	m_GFXEngine->Draw();
	m_RenderQueue->Clear();
	double u, r;
	m_GFXEngine->GetParticleTimes(u,r);

	ImGui::Begin("Particles");
	ImGui::Text("Update Time: %f", u);
	ImGui::Text("Render Time: %f", r);
	if (g_Input.IsKeyPushed(GLFW_KEY_T)) {
		m_GFXEngine->ToogleParticleCulling();
	}
	ImGui::End();

}

void SSGraphics::Shutdown() {
	delete m_GFXEngine;
}

