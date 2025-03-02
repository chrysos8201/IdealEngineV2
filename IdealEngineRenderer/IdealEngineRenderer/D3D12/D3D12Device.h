#pragma once
#include "D3D12/D3D12Thirdparty.h"
#include "D3D12/D3D12Definition.h"
#include "D3D12/D3D12Resource.h"
#include "D3D12/D3D12CommandList.h"
#include "Core/Types.h"
#include <memory>
class CD3D12DescriptorHeap;
class CD3D12CommandAllocator;
class CD3D12CommandList;
class CD3D12Resource;

struct CD3D12CommandContext
{
	std::shared_ptr<CD3D12CommandAllocator> CommandAllocator;
	std::shared_ptr<CD3D12CommandList> CommandList;

	void TransitionResource(std::shared_ptr<CD3D12Resource> Resource, D3D12_RESOURCE_STATES After);
	void Reset();
};

class CD3D12Device : public std::enable_shared_from_this<CD3D12Device>
{
	static const uint32 SWAP_CHAIN_FRAME_COUNT = G_SWAP_CHAIN_NUM;
	static const uint32 MAX_PENDING_FRAME_COUNT = SWAP_CHAIN_FRAME_COUNT - 1;

public:
	CD3D12Device(bool bWithDebug);
	~CD3D12Device();

public:
	void Init();
	void Destroy();
	uint64 GetCurrentContextIndex() const { return CurrentContextIndex; }

	ComPtr<ID3D12Device5> GetDevice() { return Device; };
	ComPtr<IDXGIAdapter1> GetAdapter() { return DxgiAdapter; }
	ComPtr<IDXGIFactory7> GetDXGIFactory7() { return DxgiFactory7; }
	ComPtr<ID3D12CommandQueue> GetCommandQueue() { return D3DCommandQueue; }

	std::shared_ptr<CD3D12DescriptorHeap> GetRTVHeap();
	std::shared_ptr<CD3D12DescriptorHeap> GetCBV_SRV_UAVHeap();
	std::shared_ptr<CD3D12DescriptorHeap> GetShaderVisibleHeap();

	uint64 Fence();
	void WaitForFenceValue(uint64 ExpectedFenceValue);

	void ResetCommandContexts();
	CD3D12CommandContext GetCommandContext() const { return CommandContexts[CurrentContextIndex]; }
	void MoveToNextContextIndex();

private:
	void CreateDevice(bool bWithDebug);
	void CreateHeaps();
	void CreateCommandContexts();

private:
	void CreateDXGIFactory(bool bWithDebug);
	void CreateDXGIAdapter();
	void CreateCommandQueue();
	void CreateFence();

private:
	bool bDebugMode;
	ComPtr<ID3D12Device5> Device;
	ComPtr<IDXGIFactory2> DxgiFactory2;
	ComPtr<IDXGIFactory7> DxgiFactory7;

	ComPtr<IDXGIAdapter1> DxgiAdapter;

	ComPtr<ID3D12CommandQueue> D3DCommandQueue;
	ComPtr<ID3D12Fence> D3DFence;
	uint64 FenceValue;
	HANDLE FenceEvent;
	uint64 LastFenceValues[MAX_PENDING_FRAME_COUNT];
	uint64 CurrentContextIndex;

	// ÃÖ´ë Descriptor ¼ö
	int32 MaxStandardDescriptors = 0;
	int32 MaxSamplerDescriptors = 0;

	std::shared_ptr<CD3D12DescriptorHeap> RTVHeap;
	std::shared_ptr<CD3D12DescriptorHeap> CBV_SRV_UAV_Heap;
	std::shared_ptr<CD3D12DescriptorHeap> ShaderVisibleHeap;

	CD3D12CommandContext CommandContexts[MAX_PENDING_FRAME_COUNT];

public:
	// Resource Manager

private:
	template <typename TVertexType>
	std::shared_ptr<CD3D12VertexBuffer> CreateVertexBuffer(std::vector<TVertexType>& Vertices)
	{
		CommandContexts[CurrentContextIndex]->Reset();

		const uint32 elementSize = sizeof(TVertexType);
		const uint32 elementCount = (uint32)Vertices.size();
		const uint32 bufferSize = elementSize * elementCount;

		CD3D12UploadBuffer uploadBuffer(Device.Get(), bufferSize);
		{
			void* mappedData = uploadBuffer.Map();
			memcpy(mappedData, Vertices.data(), bufferSize);
			uploadBuffer.UnMap();
		}
		std::shared_ptr<CD3D12VertexBuffer> OutVertexBuffer
			= std::make_shared<CD3D12VertexBuffer>(Device.Get(), bufferSize, elementSize, elementCount);

		//Execute
		CommandContexts[CurrentContextIndex].CommandList->Close();
		ID3D12CommandList* ppCommandLists[] = { CommandContexts[CurrentContextIndex].CommandList->GetGraphicsCommandList().Get() };
		D3DCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		Fence();
		WaitForFenceValue(FenceValue);

		return OutVertexBuffer;
	}

	std::shared_ptr<CD3D12IndexBuffer> CreateIndexBuffer(std::vector<uint32>& Indices);
};

