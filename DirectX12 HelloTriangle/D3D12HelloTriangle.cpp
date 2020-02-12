#include "stdafx.h"
#include "D3D12HelloTriangle.h"



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
