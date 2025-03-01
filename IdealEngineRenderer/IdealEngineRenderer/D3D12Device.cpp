#include "D3D12Device.h"
#include "D3D12Descriptors.h"
#include "D3D12Util.h"
#include "D3D12CommandList.h"
#include "D3D12Resource.h"

void CD3D12CommandContext::TransitionResource(std::shared_ptr<CD3D12Resource> Resource, D3D12_RESOURCE_STATES After)
{
	D3D12_RESOURCE_STATES before = Resource->GetResourceState();
	ID3D12Resource* d3dResource = Resource->GetResource();
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		d3dResource,
		before,
		After
	);
	CommandList->GetGraphicsCommandList()->ResourceBarrier(1, &barrier);
	// 현재 리소스 상태 변경
	Resource->SetResourceState(After);
}

CD3D12Device::CD3D12Device(bool bWithDebug)
	: Device()
	, DxgiFactory2()
	, DxgiFactory7()
	, DxgiAdapter()
	, D3DCommandQueue()
	, D3DFence()
	, FenceValue(0)
	, FenceEvent(NULL)
	, MaxStandardDescriptors(0)
	, MaxSamplerDescriptors(0)
{
	CreateDevice(bWithDebug);
	CreateHeaps();
	CreateFence();
	CreateCommandContexts();
}

CD3D12Device::~CD3D12Device()
{

}

void CD3D12Device::Destroy()
{

}

std::shared_ptr<CD3D12DescriptorHeap> CD3D12Device::GetRTVHeap()
{
	return RTVHeap;
}

std::shared_ptr<CD3D12DescriptorHeap> CD3D12Device::GetCBV_SRV_UAVHeap()
{
	return CBV_SRV_UAV_Heap;
}

std::shared_ptr<CD3D12DescriptorHeap> CD3D12Device::GetShaderVisibleHeap()
{
	return ShaderVisibleHeap;
}

uint64 CD3D12Device::Fence()
{
	FenceValue++;
	D3DCommandQueue->Signal(D3DFence.Get(), FenceValue);
	LastFenceValues[CurrentContextIndex] = FenceValue;

	return FenceValue;
}

void CD3D12Device::WaitForFenceValue(uint64 ExpectedFenceValue)
{
	uint64 completedValue = D3DFence->GetCompletedValue();
	if (D3DFence->GetCompletedValue() < ExpectedFenceValue)
	{
		D3DFence->SetEventOnCompletion(ExpectedFenceValue, FenceEvent);
		WaitForSingleObject(FenceEvent, INFINITE);
	}
	uint64 completedValue1 = D3DFence->GetCompletedValue();
}

void CD3D12Device::ResetCommandContexts()
{
	CommandContexts[CurrentContextIndex].CommandAllocator->Reset();
	std::shared_ptr<CD3D12CommandAllocator> commandAllocator = CommandContexts[CurrentContextIndex].CommandAllocator;
	CommandContexts[CurrentContextIndex].CommandList->Reset(commandAllocator);
}

void CD3D12Device::MoveToNextContextIndex()
{
	uint32 nextContextIndex = (CurrentContextIndex + 1) % MAX_PENDING_FRAME_COUNT;
	WaitForFenceValue(LastFenceValues[nextContextIndex]);
	CurrentContextIndex = nextContextIndex;
}

void CD3D12Device::CreateDevice(bool bWithDebug)
{
	if (bWithDebug)
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf()))))
		{
			debugController->EnableDebugLayer();
			
		}
	}
	
	CreateDXGIFactory(true);
	CreateDXGIAdapter(); // CreateDevice
	CreateCommandQueue();

	MaxStandardDescriptors = D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_1;
	MaxSamplerDescriptors = D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE;

}

void CD3D12Device::CreateHeaps()
{
	RTVHeap
		= std::make_shared<CD3D12DescriptorHeap>(
			shared_from_this()
			, D3D12_DESCRIPTOR_HEAP_TYPE_RTV
			, D3D12_DESCRIPTOR_HEAP_FLAG_NONE
			, 128
		);
	CBV_SRV_UAV_Heap
		= std::make_shared<CD3D12DescriptorHeap>(
			shared_from_this()
			, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
			, D3D12_DESCRIPTOR_HEAP_FLAG_NONE
			, MaxStandardDescriptors
		);

	// Main Shader Visible Heap
	ShaderVisibleHeap
		= std::make_shared<CD3D12DescriptorHeap>(
			shared_from_this()
			, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
			, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
			, MaxStandardDescriptors
		);

}

void CD3D12Device::CreateCommandContexts()
{
	//-------Command Context--------//
	for (uint32 i = 0; i < MAX_PENDING_FRAME_COUNT; i++)
	{
		CommandContexts[i].CommandAllocator = std::make_shared<CD3D12CommandAllocator>(shared_from_this(), D3D12_COMMAND_LIST_TYPE_DIRECT);
		CommandContexts[i].CommandList = std::make_shared<CD3D12CommandList>(shared_from_this(), CommandContexts[i].CommandAllocator);
		CommandContexts[i].CommandList->Close();
	}
}

void CD3D12Device::CreateDXGIFactory(bool bWithDebug)
{
	const uint32 Flags = bWithDebug ? DXGI_CREATE_FACTORY_DEBUG : 0;
	Check(
		CreateDXGIFactory2(Flags, IID_PPV_ARGS(DxgiFactory2.GetAddressOf()))
		, L"Failed To Create DXGIFactory2"
	);
	DxgiFactory2->QueryInterface(IID_PPV_ARGS(DxgiFactory7.GetAddressOf()));
}

void CD3D12Device::CreateDXGIAdapter()
{
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1
	};

	uint32 featureLevelNum = _countof(featureLevels);
	DXGI_ADAPTER_DESC1 adapterDesc = {};

	bool bFoundAdapter = false;

	for (uint32 featureLevelIndex = 0; featureLevelIndex < featureLevelNum; featureLevelIndex++)
	{
		uint32 adapterIndex = 0;
		while (DXGI_ERROR_NOT_FOUND != DxgiFactory7->EnumAdapters1(adapterIndex, DxgiAdapter.GetAddressOf()))
		{
			DxgiAdapter->GetDesc1(&adapterDesc);

			if (SUCCEEDED(D3D12CreateDevice(DxgiAdapter.Get(), featureLevels[featureLevelIndex], IID_PPV_ARGS(Device.GetAddressOf()))))
			{
				bFoundAdapter = true;
				break;
			}
		}
		if (bFoundAdapter)
		{
			break;
		}
		adapterIndex++;
	}

}

void CD3D12Device::CreateCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(D3DCommandQueue.GetAddressOf()));
}

void CD3D12Device::CreateFence()
{
	VERIFYD3D12RESULT(Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(D3DFence.GetAddressOf())););
	FenceValue = 0;
	FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}