#pragma once

#include <map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "GFXDXLibraryDefine.h"
#include "Model.h"
#include "VertexBuffer.h"
#include <d3d12.h>
/*
This class loads and store all models and vertices and indices.
It also generates vertex and index buffers.
*/
namespace gfx_dx {
typedef int ModelHandle;
#define g_ModelBank ModelBank::GetInstance()
class ModelBank {
  public:
	GFX_DX_API ~ModelBank();
	GFX_DX_API static ModelBank& GetInstance();
	GFX_DX_API const Model& FetchModel(ModelHandle handle);
	GFX_DX_API ModelHandle LoadModel(const char* filename);
	GFX_DX_API ModelHandle AddModel(Model& TheModel);
	GFX_DX_API ModelHandle CreateCustomModel(std::vector<Vertex>* vertices, std::vector<UINT32>* indices);
	GFX_DX_API void UpdateModel(ModelHandle& handle, Model& model);
	GFX_DX_API void BuildBuffers();
	GFX_DX_API void DeleteModel();
	GFX_DX_API void ApplyBuffers();
	GFX_DX_API void Clear();
	GFX_DX_API std::vector<Vertex>& GetVertices();
	GFX_DX_API  VertexBuffer& GetVertexBuffer();
	GFX_DX_API ID3D12Resource* GetIndexBuffer();
  private:
	ModelBank();
	void LoadMeshes(Model& model, const aiScene* scene);
	VertexBuffer							m_VertexBuffer;
	ID3D12Resource*							m_IndexBuffer;
	ModelHandle								m_Numerator;
	std::map<ModelHandle, Model>			m_Models;
	std::vector<Vertex>	m_Vertices;
	std::vector<UINT32>						m_Indices;
	int										m_VerticesWhenLastBuilt;
};
}
