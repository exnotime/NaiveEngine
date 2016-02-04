#include "RenderQueue.h"
#include "d3dx12.h"
gfx_dx::RenderQueue::RenderQueue() {
	m_ModelQueue.reserve(50);
	m_ShaderInputs.reserve(MAX_OBJECTS);
}

gfx_dx::RenderQueue::~RenderQueue() {

}

void gfx_dx::RenderQueue::CreateBuffer(ID3D12Device* device) {
	HRESULT hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(ShaderInput) * MAX_OBJECTS), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_ShaderInputbuffer));
}

void gfx_dx::RenderQueue::UpdateBuffer() {
	ShaderInput* m_Data;
	m_ShaderInputbuffer->Map(0, nullptr, (void**)&m_Data);
	memcpy(m_Data, m_ShaderInputs.data(), sizeof(ShaderInput) * m_ShaderInputs.size());
	m_ShaderInputbuffer->Unmap(0, nullptr);
}

ID3D12Resource* gfx_dx::RenderQueue::GetBuffer() {
	return m_ShaderInputbuffer;
}

void gfx_dx::RenderQueue::Enqueue(ModelHandle model, const std::vector<ShaderInput>& inputs) {
	ModelObject mo;
	mo.Model = model;
	mo.InstanceCount = (int)inputs.size();
	m_ModelQueue.push_back(mo);
	m_ShaderInputs.insert(m_ShaderInputs.end(), inputs.begin(), inputs.end());
}

void gfx_dx::RenderQueue::Enqueue(ModelHandle model, const ShaderInput& input) {
	ModelObject mo;
	mo.Model = model;
	mo.InstanceCount = 1;
	m_ModelQueue.push_back(mo);
	m_ShaderInputs.insert(m_ShaderInputs.end(), input);
}

void gfx_dx::RenderQueue::Clear() {
	m_ModelQueue.clear();
	m_ShaderInputs.clear();
	m_Views.clear();
}
