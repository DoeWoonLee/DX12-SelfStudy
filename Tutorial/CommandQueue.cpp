#include "DX12LibPCH.h"
#include "CommandQueue.h"


CommandQueue::CommandQueue(Microsoft::WRL::ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type)
	: m_FenceValue(0)
	, m_CommandListType(type)
	, m_d3d12Device(device)
{
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = type;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;

	ThrowIfFailed(m_d3d12Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_d3d12CommandQueue)));
	ThrowIfFailed(m_d3d12Device->CreateFence(m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_d3d12Fence)));

	m_FenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(m_FenceEvent && "Failed to create fence event handle.");
}

CommandQueue::~CommandQueue()
{
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> CommandQueue::GetCommandList(void)
{
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList;

	// there is at least one item in the command allocator queue
	// there is the fence value associated with that command allocator has been reached.
	if (!m_CommandAllocatorQueue.empty() && IsFenceComplete(m_CommandAllocatorQueue.front().fenceValue))
	{
		commandAllocator = m_CommandAllocatorQueue.front().commandAllocator;
		m_CommandAllocatorQueue.pop();

		ThrowIfFailed(commandAllocator->Reset());
	}
	else
	{
		commandAllocator = CreateCommandAllocator();
	}

	if (!m_CommandListQueue.empty())
	{
		commandList = m_CommandListQueue.front();
		m_CommandListQueue.pop();

		ThrowIfFailed(commandList->Reset(commandAllocator.Get(), nullptr));
	}
	else
	{
		commandList = CreateCommandList(commandAllocator);
	}

	// Associated the command allocator with the command list so that it can be
	// retrieved when the command list is executed.
	ThrowIfFailed(commandList->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), commandAllocator.Get()));

	/*
	It should be noted that when assigning a COM object to the private
	data of a ID3D12Object object using the ID3D12Object::SetPrivateDataInterface
	method, the internal reference counter of the assigned COM object is incremented.
	The ref counter of the assigned COM object is only decremented if either
	the owning ID3D12Object object is destroyed or the instance of the
	COM object with the same interface is replaced with another COM
	object of the same interface or a NULL pointer.
	*/

	return commandList;
}

uint64_t CommandQueue::ExecuteCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
{
	// Before the command list can be executed, it must be closed.
	commandList->Close();

	ID3D12CommandAllocator* commandAllocator;
	UINT dataSize = sizeof(commandAllocator);

	// You should be aware that retrieving a COM pointer of a COM object
	// associated with the private data of the ID3D12Object object will also increment
	// the reference counter of that COM Object.
	ThrowIfFailed(commandList->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &commandAllocator));

	ID3D12CommandList* const ppCommandLists[] = {
		commandList.Get()
	};

	m_d3d12CommandQueue->ExecuteCommandLists(1, ppCommandLists);
	// Immediately after executing the command list, the queue is signaled using the Signal 
	// method. This produces the fence value to wait for to ensure that the command
	// allocators can be reused.
	uint64_t fenceValue = Signal();

	// they can be reused the next time the GetCommandList method is called.
	m_CommandAllocatorQueue.emplace(CommandAllocatorEntry{ fenceValue, commandAllocator });
	m_CommandListQueue.push(commandList);

	// The ownership of the command allocator has been transferred to the ComPtr
	// in the command allocator queue. It is safe to release the reference
	// in this temporary COM pointer here.
	commandAllocator->Release();


	return fenceValue;
}

uint64_t CommandQueue::Signal(void)
{
	uint64_t fenceValueForSignal = ++m_FenceValue;
	ThrowIfFailed(m_d3d12CommandQueue->Signal(m_d3d12Fence.Get(), fenceValueForSignal));

	return fenceValueForSignal;
}

bool CommandQueue::IsFenceComplete(uint64_t fenceValue)
{
	return m_d3d12Fence->GetCompletedValue() >= fenceValue;
}

void CommandQueue::WaitForFenceValue(uint64_t fenceValue)
{
	if (m_d3d12Fence->GetCompletedValue() < fenceValue)
	{
		ThrowIfFailed(m_d3d12Fence->SetEventOnCompletion(fenceValue, m_FenceEvent));
		::WaitForSingleObject(m_FenceEvent, static_cast<DWORD>(std::chrono::milliseconds::max().count() ));
	}
}

void CommandQueue::Flush()
{
	uint64_t fenceValueForSingal = Signal();
	WaitForFenceValue(m_FenceValue);
}

Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue::GetD3D12CommandQueue() const
{
	return m_d3d12CommandQueue;
}

Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandQueue::CreateCommandAllocator()
{
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	ThrowIfFailed(m_d3d12Device->CreateCommandAllocator(m_CommandListType, IID_PPV_ARGS(&commandAllocator)));

	return commandAllocator;
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> CommandQueue::CreateCommandList(Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator)
{
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList;
	ThrowIfFailed(m_d3d12Device->CreateCommandList(0, m_CommandListType, allocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));

	return commandList;
}
