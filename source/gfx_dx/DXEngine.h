#pragma once

#include "d3dx12.h"

#include <wrl/client.h>
#include <glm/glm.hpp>

#include "GFXDXLibraryDefine.h"

using Microsoft::WRL::ComPtr;
class IDXGISwapChain3;
class ID3D12Device;
class ID3D12CommandAllocator;
class ID3D12Resource;
class ID3D12CommandQueue;
class ID3D12DescriptorHeap;
class ID3D12PipelineState;
class ID3D12GraphicsCommandList;
class ID3D12RootSignature;
class ID3D12Fence;

namespace gfx_dx {
	struct Vertex {
		glm::vec3 Position;
		glm::vec2 TexCoord;
	};

	struct cbuffer {
		glm::mat4 wvp;
	};

	class GFX_DX_API DXengine {
	public:
		DXengine();
		~DXengine();
		 void Init(HWND hWnd, int width, int height);
		 void Update();
		 void Render();
		 void Swap();
	private:
		void LoadAssets();
		D3D12_VIEWPORT m_viewport;
		D3D12_RECT m_scissorRect;
		static const UINT FrameCount = 2;

		ComPtr<IDXGISwapChain3> m_SwapChain;
		ComPtr<ID3D12Device> m_Device;
		ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
		ComPtr<ID3D12Resource> m_RenderTargets[FrameCount];
		ComPtr<ID3D12CommandQueue> m_CommandQueue;
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		ComPtr<ID3D12DescriptorHeap> m_CbvSrvUavHeap;
		ComPtr<ID3D12DescriptorHeap> m_SamplerHeap;
		ComPtr<ID3D12PipelineState> m_Pipeline;
		ComPtr<ID3D12GraphicsCommandList> m_GFXCommandList;
		ComPtr<ID3D12RootSignature> m_RootSignature;
		ComPtr<ID3D12Resource> m_VBO;
		D3D12_VERTEX_BUFFER_VIEW m_VBOView;
		ComPtr<ID3D12Resource> m_IBO;
		D3D12_INDEX_BUFFER_VIEW m_IBOView;
		ComPtr<ID3D12Resource> m_Texture;
		ComPtr<ID3D12Resource> m_ConstantBuffer;
		cbuffer	m_CBufferData;
		unsigned char* m_CBufferBegin;

		int m_Width;
		int m_Height;
		unsigned int m_FrameIndex;
		unsigned int m_RTVHeapSize;
		unsigned int m_HeapDescSize;
		HANDLE m_FenceEvent;
		ComPtr<ID3D12Fence> m_Fence;
		UINT64 m_FenceValue;

	};
}
