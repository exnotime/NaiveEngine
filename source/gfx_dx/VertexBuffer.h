#pragma once
#include "Vertex.h"
#include <d3d12.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
namespace gfx_dx {
	class VertexBuffer {
	public:
		VertexBuffer();
		~VertexBuffer();
		void Init(ID3D12Device* device, void* Data, unsigned int vertexCount);
		void Apply(int slot, ID3D12GraphicsCommandList* cmdList);
		void* Map();
		void UnMap();
		void Release();
	private:
		ComPtr<ID3D12Resource> m_VertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
	};
}