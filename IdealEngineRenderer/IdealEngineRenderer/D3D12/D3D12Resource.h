#pragma once
#include "Core/Core.h"
#include "D3D12/D3D12Thirdparty.h"
#include "D3D12/D3D12Descriptors.h"

class CD3D12DescriptorHandle;

class CD3D12Resource
{
public:
	CD3D12Resource(ComPtr<ID3D12Resource> InResource);
	CD3D12Resource(uint32 InBufferSize);
	virtual ~CD3D12Resource();

public:
	void SetName(const LPCTSTR& Name);

	ID3D12Resource* GetResource() const;

	inline uint32 GetBufferSize() const { return BufferSize; };

	void SetResourceState(D3D12_RESOURCE_STATES After) { CurrentResourceState = After; }
	D3D12_RESOURCE_STATES GetResourceState() const { return CurrentResourceState; }

public:
	void EmplaceSRV(const CD3D12DescriptorHandle& InSRVHandle);
	void EmplaceRTV(const CD3D12DescriptorHandle& InRTVHandle);
	void EmplaceUAV(const CD3D12DescriptorHandle& InUAVHandle);

	CD3D12DescriptorHandle GetSRV() const;
	CD3D12DescriptorHandle GetRTV() const;
	CD3D12DescriptorHandle GetUAV() const;

private:
	CD3D12DescriptorHandle RTVHandle;
	CD3D12DescriptorHandle SRVHandle;
	CD3D12DescriptorHandle UAVHandle;

	D3D12_RESOURCE_STATES CurrentResourceState;

protected:
	ComPtr<ID3D12Resource> Resource;
	uint32 BufferSize;
	std::wstring Name;
};

class CD3D12UploadBuffer : public CD3D12Resource
{
	void EmplaceSRV(const CD3D12DescriptorHandle& InSRVHandle) = delete;
	void EmplaceRTV(const CD3D12DescriptorHandle& InRTVHandle) = delete;
	void EmplaceUAV(const CD3D12DescriptorHandle& InUAVHandle) = delete;

	CD3D12DescriptorHandle GetSRV() = delete;
	CD3D12DescriptorHandle GetRTV() = delete;
	CD3D12DescriptorHandle GetUAV() = delete;

public:
	CD3D12UploadBuffer(ID3D12Device* Device, uint32 InBufferSize);
	virtual ~CD3D12UploadBuffer();

	void* Map();
	void UnMap();



};

class CD3D12GPUBuffer : public CD3D12Resource
{
public:
	CD3D12GPUBuffer(ID3D12Device* Device, uint32 InElementSize, uint32 InElementCount, D3D12_RESOURCE_FLAGS Flags = D3D12_RESOURCE_FLAG_NONE);

public:
	inline uint32 GetElementCount() const { return ElementCount; };
	inline uint32 GetElementSize() const { return ElementSize; }

private:
	uint32 BufferSize;
	uint32 ElementSize;
	uint32 ElementCount;
};