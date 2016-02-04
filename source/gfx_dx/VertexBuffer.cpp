#include "VertexBuffer.h"
#include "d3dx12.h"
using namespace gfx_dx;
VertexBuffer::VertexBuffer() {

}
VertexBuffer::~VertexBuffer() {

}

void VertexBuffer::Init(ID3D12Device* device, void* Data, unsigned int vertexCount) {
	UINT32 sizeInBytes = vertexCount * sizeof(Vertex);
	HRESULT hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeInBytes), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_VertexBuffer));

	UINT8* vertexDataBegin;
	m_VertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataBegin));
	memcpy(vertexDataBegin, Data, sizeInBytes);
	m_VertexBuffer->Unmap(0, nullptr);

	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = sizeof(Vertex);
	m_VertexBufferView.SizeInBytes = sizeInBytes;
}

void VertexBuffer::Apply(int slot, ID3D12GraphicsCommandList* cmdList) {
	cmdList->IASetVertexBuffers(slot, 1, &m_VertexBufferView);
}

void* VertexBuffer::Map() {
	UINT8* vertexDataBegin;
	m_VertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataBegin));
	return vertexDataBegin;
}

void VertexBuffer::UnMap() {
	m_VertexBuffer->Unmap(0, nullptr);
}

void VertexBuffer::Release() {
}