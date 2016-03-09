#include "DXEngine.h"
#include <Soil2/SOIL2.h>
#include <glm/gtx/transform.hpp>
#include <d3dcompiler.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <stdio.h>
gfx_dx::DXengine::DXengine(){
}

gfx_dx::DXengine::~DXengine(){

}

void gfx_dx::DXengine::Init(HWND hWnd, int width, int height){
	m_Width = width;
	m_Height = height;
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.MinDepth = 0;
	m_viewport.Width = (float)width;
	m_viewport.Height = (float)height;
	m_viewport.MaxDepth = 1.0f;

	m_scissorRect.right = width;
	m_scissorRect.bottom = height;
	m_scissorRect.left = 0;
	m_scissorRect.top = 0;

	HRESULT hr;
	//set up debug
#ifdef _DEBUG
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
			debugController->EnableDebugLayer();
		}
	}
#endif

	ComPtr<IDXGIFactory> dxgiFactory;
	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	//create device
	hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_Device));

	//create command queue
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	hr = m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue));

	//create swapchain
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = FrameCount;
	swapChainDesc.BufferDesc.Width = m_Width;
	swapChainDesc.BufferDesc.Height = m_Height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = TRUE;

	ComPtr<IDXGISwapChain> swapchain;
	hr = dxgiFactory->CreateSwapChain(m_CommandQueue.Get(), &swapChainDesc, &swapchain);
	swapchain.As(&m_SwapChain);
	//keep track off which frame we are on
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	//create a descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = FrameCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 2;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	hr = m_Device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_CbvSrvUavHeap));

	//get size of descriptors
	m_RTVHeapSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_HeapDescSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//get handle for first descriptor
	CD3DX12_CPU_DESCRIPTOR_HANDLE  rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (int i = 0; i < FrameCount; i++) {
		m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i]));
		m_Device->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, m_RTVHeapSize);
	}
	m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator));

	LoadAssets();

}

void gfx_dx::DXengine::Update() {
	static float angle = 0.0f;
	angle += 0.01f;
	glm::mat4 wvp;
	wvp = glm::perspectiveFov(1.2f, 1600.0f, 900.0f, 0.1f, 100.0f) * glm::lookAt(glm::vec3(0.8f,0.3f, 2.0f), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)) * (glm::scale(glm::vec3(0.5f)) * glm::rotate(angle, glm::vec3(0,1,0)));
	wvp = glm::transpose(wvp);
	//upload cbuffer

}

void gfx_dx::DXengine::Render(){
	HRESULT hr = S_OK;
	//reset cmdlist
	hr = m_CommandAllocator->Reset();
	hr = m_GFXCommandList->Reset(m_CommandAllocator.Get(), m_Pipeline.Get());
	//set state
	m_GFXCommandList->SetGraphicsRootSignature(m_RootSignature.Get());
	//set descriptor table
	ID3D12DescriptorHeap* ppHeaps[] = { m_CbvSrvUavHeap.Get()};
	m_GFXCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	m_GFXCommandList->SetGraphicsRootDescriptorTable(0, m_CbvSrvUavHeap->GetGPUDescriptorHandleForHeapStart());
	//set viewport and rect
	m_GFXCommandList->RSSetViewports(1, &m_viewport);
	m_GFXCommandList->RSSetScissorRects(1, &m_scissorRect);

	m_GFXCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RTVHeapSize);
	m_GFXCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	//record cmds
	const float clearColor[] = {0.3f,0.4f,0.5f,1.0f};
	m_GFXCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	m_GFXCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_GFXCommandList->IASetVertexBuffers(0, 1, &m_VBOView);
	m_GFXCommandList->IASetIndexBuffer(&m_IBOView);
	m_GFXCommandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
	//close cmdlist
	m_GFXCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	hr = m_GFXCommandList->Close();
	//execute cmdlist
	ID3D12CommandList* ppCommandLists[] = { m_GFXCommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}

void gfx_dx::DXengine::Swap(){
	m_SwapChain->Present(1, 0);
	//wait for it to finish
	const UINT64 fence = m_FenceValue;
	m_CommandQueue->Signal(m_Fence.Get(), fence);
	m_FenceValue++;
	if (m_Fence->GetCompletedValue() < fence) {
		m_Fence->SetEventOnCompletion(fence, m_FenceEvent);
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
}

void gfx_dx::DXengine::LoadAssets(){
	HRESULT hr;
	//create root signature
	{
		//descriptor table 
		CD3DX12_DESCRIPTOR_RANGE ranges[2];
		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
		ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);

		CD3DX12_ROOT_PARAMETER rootParameters[1];
		rootParameters[0].InitAsDescriptorTable(2, &ranges[0], D3D12_SHADER_VISIBILITY_ALL);

		//create a static sampler
		CD3DX12_STATIC_SAMPLER_DESC sampDesc = {};
		sampDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		sampDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		sampDesc.MaxAnisotropy = 1;
		sampDesc.MaxLOD = D3D12_FLOAT32_MAX;
		sampDesc.MinLOD = 0;
		sampDesc.MipLODBias = 0;
		sampDesc.RegisterSpace = 0;
		sampDesc.ShaderRegister = 0;
		sampDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		CD3DX12_ROOT_SIGNATURE_DESC rootSignDesc;
		rootSignDesc.Init(_countof(rootParameters), rootParameters, 1, &sampDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> sign;
		ComPtr<ID3DBlob> error;
		hr = D3D12SerializeRootSignature(&rootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sign, &error);
		if (error) {
			printf("Root signature error: %s\n", (char*)error->GetBufferPointer());
		}
		hr = m_Device->CreateRootSignature(0, sign->GetBufferPointer(), sign->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
	}
	//create pipeline state
	{
		//compile shaders
		ComPtr<ID3DBlob> vertexShader;
		ComPtr<ID3DBlob> pixelShader;

#ifdef _DEBUG
		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		UINT compileFlags = 0;
#endif
		ComPtr<ID3DBlob> vsErrors;
		ComPtr<ID3DBlob> psErrors;
		hr = D3DCompileFromFile(L"Shader/hlsl/Color.hlsl", nullptr, nullptr, "VSMain", "vs_5_1", compileFlags, 0, &vertexShader, &vsErrors);
		hr = D3DCompileFromFile(L"Shader/hlsl/Color.hlsl", nullptr, nullptr, "PSMain", "ps_5_1", compileFlags, 0, &pixelShader, &psErrors);
		if (vsErrors) {
			printf("Errors in vertex shader\n %s\n", (char*)vsErrors->GetBufferPointer());
		}
		if (psErrors) {
			printf("Errors in pixel shader\n %s\n", (char*)psErrors->GetBufferPointer());
		}
		//create input layout
		D3D12_INPUT_ELEMENT_DESC inputDesc[] = 
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};
		//Create pipeline state object
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { inputDesc, _countof(inputDesc) };
		psoDesc.pRootSignature = m_RootSignature.Get();
		psoDesc.VS = { reinterpret_cast<UINT8*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
		psoDesc.PS = { reinterpret_cast<UINT8*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		hr = m_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_Pipeline));
	}

	hr = m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), m_Pipeline.Get() , IID_PPV_ARGS(&m_GFXCommandList));
	
	ComPtr<ID3D12Resource> textureUploadHeap;
	unsigned char* texData;
	//create texture
	{
		int w, h, c;
		//load tex
		texData = SOIL_load_image("asset/zoid.jpg", &w, &h, &c, SOIL_LOAD_RGBA);
		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.MipLevels = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.Width = w;
		textureDesc.Height = h;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		m_Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&textureDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&m_Texture));

		const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_Texture.Get(), 0, 1);

		// Create the GPU upload buffer.
		m_Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&textureUploadHeap));

		D3D12_SUBRESOURCE_DATA textureData = {};
		textureData.pData = texData;
		textureData.RowPitch = w * 4;
		textureData.SlicePitch = textureData.RowPitch * h;

		UpdateSubresources(m_GFXCommandList.Get(), m_Texture.Get(), textureUploadHeap.Get(), 0, 0, 1, &textureData);
		m_GFXCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		D3D12_SHADER_RESOURCE_VIEW_DESC srvView = {};
		srvView.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvView.Format = textureDesc.Format;
		srvView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvView.Texture2D.MipLevels = 1;
		
		m_Device->CreateShaderResourceView(m_Texture.Get(), &srvView, m_CbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart());
	}
	//create cbuffer
	hr = m_Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(PerFrame)), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_PerFrameBuffer));

	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_CbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart());
	handle.Offset(2, m_HeapDescSize);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = m_PerFrameBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = sizeof(PerFrame);
	m_Device->CreateConstantBufferView(&cbvDesc, handle);

	//execute cmdlist
	hr = m_GFXCommandList->Close();
	ID3D12CommandList* ppCommandList[] = { m_GFXCommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(1, ppCommandList);

	//create fence to know when the gpu has finished working
	hr = m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
	m_FenceValue = 1;
	m_FenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
	if (m_FenceEvent) {
		//something went wrong
	}
	//wait for the GPU to finish
	const UINT64 fence = m_FenceValue;
	m_CommandQueue->Signal(m_Fence.Get(), fence);
	m_FenceValue++;

	// Wait until the previous frame is finished.
	if (m_Fence->GetCompletedValue() < fence)
	{
		m_Fence->SetEventOnCompletion(fence, m_FenceEvent);
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}

	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
	SOIL_free_image_data(texData);
}

gfx_dx::RenderQueue* gfx_dx::DXengine::GetRenderQueue() {
	return m_RenderQueue;
}