#include "RenderQueue.h"
gfx_dx::RenderQueue::RenderQueue() {
	m_ModelQueue.reserve(50);
	m_ShaderInputs.reserve(MAX_OBJECTS);
}

gfx_dx::RenderQueue::~RenderQueue() {

}

void gfx_dx::RenderQueue::CreateBuffer(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE descHandle) {

	HRESULT hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ShaderInput) * MAX_OBJECTS)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_ShaderInputbuffer)
		);

	D3D12_BUFFER_SRV srvBuffer;
	srvBuffer.FirstElement = 0;
	srvBuffer.NumElements = MAX_OBJECTS;
	srvBuffer.StructureByteStride = sizeof(ShaderInput);
	srvBuffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Buffer = srvBuffer;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	device->CreateShaderResourceView(m_ShaderInputbuffer.Get(), &srvDesc, descHandle);
}

void gfx_dx::RenderQueue::UpdateBuffer() {
	ShaderInput* m_Data;
	m_ShaderInputbuffer->Map(0, nullptr, (void**)&m_Data);
	memcpy(m_Data, m_ShaderInputs.data(), sizeof(ShaderInput) * m_ShaderInputs.size());
	m_ShaderInputbuffer->Unmap(0, nullptr);
}

ID3D12Resource* gfx_dx::RenderQueue::GetBuffer() {
	return m_ShaderInputbuffer.Get();
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
