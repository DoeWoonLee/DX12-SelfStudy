#include "stdafx.h"
#include "D3D12HelloTriangle.h"
#include "Win32Application.h"


D3D12HelloTriangle::D3D12HelloTriangle(UINT width, UINT height, std::wstring name) :
	DXSample(width, height, name),
	m_frameIndex(0),
	m_rtvDescriptorSize(0)
{
}

void D3D12HelloTriangle::OnInit()
{
	LoadPipeline();
	LoadAssets();
}

void D3D12HelloTriangle::OnUpdate()
{
}

void D3D12HelloTriangle::OnRender()
{
}

void D3D12HelloTriangle::OnDestroy()
{

}

void D3D12HelloTriangle::LoadPipeline()
{
	UINT dxgiFactoryFlags = 0;
	// Debug layer
#if defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();

			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}

	}
#endif
	// DXGI Factory
	ComPtr<IDXGIFactory4> pFactory = nullptr;
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&pFactory)));
	// DXGI Adpater
	if (m_useWarpDevice) // Windows Advanced Rasterization Platform https://docs.microsoft.com/en-us/windows/win32/direct3darticles/directx-warp
	{
		ComPtr<IDXGIAdapter> pWarpAdapter = nullptr;
		pFactory->EnumAdapters(0, &pWarpAdapter);
		// Create Device
		ThrowIfFailed(D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));
	}
	else
	{
		ComPtr<IDXGIAdapter1> pAdapter = nullptr;
		GetHardwareAdapter(pFactory.Get(), &pAdapter);
		// Create Device
		ThrowIfFailed(D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));
	}
	
	// DXGI Output

	// Command Queue
	D3D12_COMMAND_QUEUE_DESC queueDesc; ZeroMemory(&queueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE; // 기본 커멘드 큐
	queueDesc.NodeMask = 0; // For Multi GPU
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
	// Create Swap Chain
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc; ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	swapChainDesc.Height = m_height;
	swapChainDesc.Width = m_width;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferCount = FrameCount;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	ComPtr<IDXGISwapChain1> pSwapChain;
	ThrowIfFailed(pFactory->CreateSwapChainForHwnd(m_commandQueue.Get(),
		Win32Application::GetHwnd(), &swapChainDesc, nullptr, nullptr, &pSwapChain));
	// This sample does not support fullscreen transitions.
	ThrowIfFailed(pFactory->MakeWindowAssociation(Win32Application::GetHwnd(), DXGI_MWA_NO_ALT_ENTER));

	// Dependency injection
	ThrowIfFailed(pSwapChain.As(&m_swapChain));
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	// Descriptor Heap
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		// For single-adapter operation, set this to zero. 
		// If there are multiple adapter nodes, set a bit to identify the node
		rtvHeapDesc.NodeMask = 0;
		rtvHeapDesc.NumDescriptors = FrameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

		ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

		m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}
	// Create Frame Resource
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV for each frame
		for (UINT n = 0; n < FrameCount; ++n)
		{
			ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
			m_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_rtvDescriptorSize);
		}
	}

	ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
}

void D3D12HelloTriangle::LoadAssets()
{
}

void D3D12HelloTriangle::PopulateCommandList()
{
}

void D3D12HelloTriangle::WaitForPreviousFrame()
{
}
