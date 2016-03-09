#include "ModelBank.h"
#include "d3dx12.h"
using namespace gfx_dx;

ModelBank::ModelBank() {
	m_Numerator	= 0;
	//create input layout
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	memcpy(m_InputLayout, &inputLayout, sizeof(inputLayout));
}

ModelBank::~ModelBank() {
	this->Clear();
}

ModelBank& ModelBank::GetInstance() {
	static ModelBank m_Bank;
	return m_Bank;
}

ModelHandle ModelBank::LoadModel(const char* filename) {
	//check if we already has a model with same filename
	for (auto& it : m_Models) {
		if (it.second.Name == filename)
			return it.first;
	}
	Assimp::Importer loader;
	Model model;
	const aiScene* scene = loader.ReadFile( filename, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace );
	if (scene && scene->HasMeshes()) {
		model.VertexHandle = (int)m_Vertices.size();
		LoadMeshes(model, scene);
		model.MaterialOffset = 0;// g_MaterialBank.GetMaterialCount();
		model.Name = std::string(filename);
		//g_MaterialBank.LoadMaterials(model, filename, scene);
	} else if (!scene) {
		printf("error loading model: %s \n", filename);
		return -1;
	}
	printf("Loaded Model %s\n", filename);
	model.Loaded = true;
	ModelHandle handle = ++m_Numerator;
	m_Models[handle] = model;
	return handle;
}

void ModelBank::LoadMeshes(Model& model, const aiScene* scene) {
	int size = 0;
	int indices = 0;
	Mesh modelMesh;
	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
		const aiMesh* mesh = scene->mMeshes[i];
		modelMesh.VertexBufferOffset = size;
		unsigned int numVertices = 0;
		unsigned int numIndices = 0;
		//foreach vertice
		for (unsigned int v = 0; v < mesh->mNumVertices; ++v) {
			Vertex vertex;
			vertex.Position.x = mesh->mVertices[v].x;
			vertex.Position.y = mesh->mVertices[v].y;
			vertex.Position.z = mesh->mVertices[v].z;
			if (mesh->HasNormals()) {
				vertex.Normal.x = mesh->mNormals[v].x;
				vertex.Normal.y = mesh->mNormals[v].y;
				vertex.Normal.z = mesh->mNormals[v].z;
			}
			if (mesh->HasTangentsAndBitangents()) {
				vertex.Tangent.x = mesh->mTangents[v].x;
				vertex.Tangent.y = mesh->mTangents[v].y;
				vertex.Tangent.z = mesh->mTangents[v].z;
			}
			if (mesh->mTextureCoords[0] != NULL) {
				vertex.TexCoord.x = mesh->mTextureCoords[0][v].x;
				vertex.TexCoord.y = mesh->mTextureCoords[0][v].y;
			}
			numVertices++;
			m_Vertices.push_back(vertex);
		}//end foreach vertice
		//Indices
		for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
			//index = (Num vertices from the already loaded models) + (Size of all the already loaded meshes + mesh->faceindices)
			m_Indices.push_back(model.VertexHandle + size + mesh->mFaces[f].mIndices[0]);
			m_Indices.push_back(model.VertexHandle + size + mesh->mFaces[f].mIndices[1]);
			m_Indices.push_back(model.VertexHandle + size + mesh->mFaces[f].mIndices[2]);
			numIndices += 3;
		}
		modelMesh.Material = mesh->mMaterialIndex;
		modelMesh.IndexBufferOffset = indices;
		size += numVertices;
		indices += numIndices;
		modelMesh.Size = numVertices;
		modelMesh.Indices = numIndices;
		model.Meshes.push_back(modelMesh);
	}//end foreach mesh
	model.NumVertices = size;
	model.NumIndices = indices;
}

ModelHandle ModelBank::AddModel( Model& TheModel ) {
	ModelHandle id = ++m_Numerator;
	//ModelHandle id = ++m_Numerators[TheModel.Type];
	m_Models[id] = TheModel;
	return id;
}

ModelHandle ModelBank::CreateCustomModel( std::vector<Vertex>* vertices, std::vector<UINT32>* indices) {
	ModelHandle id = ++m_Numerator;
	Model model;
	Mesh mesh;
	mesh.Indices = indices->size();
	mesh.Size = vertices->size();
	mesh.Material = 0;
	mesh.IndexBufferOffset = 0;
	mesh.VertexBufferOffset = 0;
	model.Meshes.push_back(mesh);
	model.MaterialOffset = 0; //make sure there is a default material loaded
	model.VertexHandle = (int)m_Vertices.size();
	model.IndexHandle = 0;
	model.NumIndices = indices->size();
	model.NumVertices = vertices->size();
	model.Loaded = true;
	//copy and offset indices
	for (unsigned int i = 0; i < indices->size(); ++i) {
		m_Indices.push_back(model.VertexHandle + indices->at(i));
	}
	//copy vertices
	m_Vertices.insert(m_Vertices.end(), vertices->begin(), vertices->end());
	m_Models[id] = model;
	return id;
}

void ModelBank::DeleteModel( ) {
	//TODOHJ: Delete model data ,vertices and indices.
	//then update all the other models after it in the memory.
	// tbh its much easier and less cumbersome to just delete all models and load them in again.
}

const Model& ModelBank::FetchModel(ModelHandle handle) {
	return m_Models[handle];
}

void ModelBank::Clear() {
	m_Models.clear();
	m_Vertices.clear();
	m_Indices.clear();
	m_Numerator = 0;
	m_Vertices.clear();
	m_Indices.clear();
}

void ModelBank::BuildBuffers(ID3D12Device* device) {
	if (m_VerticesWhenLastBuilt == (int)m_Vertices.size())
		return;
	m_VerticesWhenLastBuilt = (int)m_Vertices.size();
	if ((int)m_Vertices.size() <= 0) {
		return;
	}
	//// Vertex buffers
	m_VertexBuffer.Init(device, &m_Vertices[0], (int)m_Vertices.size());
	////IndexBuffers
	int staticId = 0;
	for (std::map<ModelHandle, Model>::iterator it = m_Models.begin(); it != m_Models.end(); ++it) {
		it->second.IndexHandle = staticId;
		staticId += it->second.NumIndices;
	}
	HRESULT hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(m_Indices.size() * sizeof(UINT32)), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_IndexBuffer));
	UINT32* data;
	m_IndexBuffer->Map(0, nullptr, (void**)&data);
	memcpy(data, m_Indices.data(), m_Indices.size() * sizeof(UINT32));
	m_IndexBuffer->Unmap(0, nullptr);

	m_IBOView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
	m_IBOView.Format = DXGI_FORMAT_R32_UINT;
	m_IBOView.SizeInBytes = m_Indices.size() * sizeof(UINT32);


}

void ModelBank::ApplyBuffers(ID3D12GraphicsCommandList* cmdList) {
	m_VertexBuffer.Apply(0, cmdList);
	cmdList->IASetIndexBuffer(&m_IBOView);
}

std::vector<Vertex>& ModelBank::GetVertices() {
	return m_Vertices;
}

VertexBuffer& ModelBank::GetVertexBuffer( ) {
	return m_VertexBuffer;
}

ID3D12Resource* ModelBank::GetIndexBuffer( ) {
	return m_IndexBuffer;
}

void ModelBank::UpdateModel(ModelHandle& handle, Model& model) {
	m_Models[handle] = model;
}

D3D12_INPUT_LAYOUT_DESC* ModelBank::GetInputLayout() {
	return m_InputLayout;
}

int ModelBank::GetInputLayoutCount() {
	return _countof(m_InputLayout);
}