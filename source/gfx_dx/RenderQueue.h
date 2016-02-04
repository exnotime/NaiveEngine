#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "GFXDXLibraryDefine.h"
#include <d3d12.h>
namespace gfx_dx {
typedef int ModelHandle;

struct ModelObject {
	ModelHandle Model;
	unsigned int InstanceCount;
};

struct ShaderInput {
	glm::mat4 World;
	glm::vec4 Color;
};

struct CameraData {
	glm::mat4 View = glm::mat4(1);
	glm::mat4 Proj = glm::mat4(1);
	glm::mat4 ProjView = glm::mat4(1);
	glm::vec3 Position = glm::vec3(0);

	glm::vec3 Forward = glm::vec3(0,0,-1);
	glm::vec3 Right = glm::vec3(1,0,0);
	glm::vec3 Up = glm::vec3(0,1,0);
	float Fov = 0.61f;
	float Near = 0.1f;
	float Far = 1000.0f;
	uint32_t Width = 16;
	uint32_t Height = 9;
};

struct Viewport {
	float x;
	float y;
	float width;
	float height;
};

struct View {
	CameraData camera;
	Viewport viewport;
};

class RenderQueue {
  public:
	RenderQueue();
	~RenderQueue();
	GFX_DX_API void Enqueue(ModelHandle model, const std::vector<ShaderInput>& inputs);
	GFX_DX_API void Enqueue(ModelHandle model, const ShaderInput& input);
	GFX_DX_API void Clear();
	void CreateBuffer(ID3D12Device* device);
	void UpdateBuffer();
	ID3D12Resource* GetBuffer();

	void AddView(const View& v) {
		m_Views.push_back(v);
	}
	const std::vector<ModelObject>& GetModelQueue() const {
		return m_ModelQueue;
	}
	const std::vector<View>& GetViews() const {
		return m_Views;
	}

  private:
	std::vector<ModelObject>			m_ModelQueue;
	std::vector<View>					m_Views;
	std::vector<ShaderInput>			m_ShaderInputs;
	ID3D12Resource*						m_ShaderInputbuffer;
	const int							MAX_OBJECTS = 1000; //change this to increase the number of items to render //also need to update shaders
	const int							SIZE_OF_OBJECT = sizeof(ShaderInput);
};
}