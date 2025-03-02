#include "D3D12/D3D12Resource.h"
#include "D3D12/D3D12Util.h"

CD3D12Resource::CD3D12Resource(uint32 InBufferSize)
	: Resource()
	, BufferSize(InBufferSize)
	, Name()
	, RTVHandle()
	, SRVHandle()
	, UAVHandle()
	, CurrentResourceState(D3D12_RESOURCE_STATE_COMMON)
{

}

CD3D12Resource::CD3D12Resource(ComPtr<ID3D12Resource> InResource)
	: Resource(InResource)
	, BufferSize()
	, Name()
	, RTVHandle()
	, SRVHandle()
	, UAVHandle()
	, CurrentResourceState(D3D12_RESOURCE_STATE_COMMON)
{

}

//CD3D12Resource::~CD3D12Resource() = default;
CD3D12Resource::~CD3D12Resource()
{

}

void CD3D12Resource::SetName(const LPCTSTR& InName)
{
	Resource->SetName(InName);
	Name = std::wstring(InName);
}

ID3D12Resource* CD3D12Resource::GetResource() const
{
	return Resource.Get();
}

void CD3D12Resource::Destroy()
{
	RTVHandle.Free();
	SRVHandle.Free();
	UAVHandle.Free();
}

void CD3D12Resource::EmplaceSRV(const CD3D12DescriptorHandle& InSRVHandle)
{
	SRVHandle = InSRVHandle;
}

void CD3D12Resource::EmplaceRTV(const CD3D12DescriptorHandle& InRTVHandle)
{
	RTVHandle = InRTVHandle;
}

void CD3D12Resource::EmplaceUAV(const CD3D12DescriptorHandle& InUAVHandle)
{
	UAVHandle = InUAVHandle;
}

CD3D12DescriptorHandle CD3D12Resource::GetSRV() const
{
	return SRVHandle;
}

CD3D12DescriptorHandle CD3D12Resource::GetRTV() const
{
	return RTVHandle;
}

CD3D12DescriptorHandle CD3D12Resource::GetUAV() const
{
	return UAVHandle;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// CD3D12UploadBuffer

CD3D12UploadBuffer::CD3D12UploadBuffer(ID3D12Device* Device, uint32 InBufferSize)
	: CD3D12Resource(InBufferSize)
{
	CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(BufferSize);

	VERIFYD3D12RESULT(Device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(Resource.GetAddressOf()
		)));
}

CD3D12UploadBuffer::~CD3D12UploadBuffer() = default;

void* CD3D12UploadBuffer::Map()
{
	void* beginData;
	VERIFYD3D12RESULT(Resource->Map(0, nullptr, &beginData));
	return beginData;
}

void CD3D12UploadBuffer::UnMap()
{
	Resource->Unmap(0, nullptr);
}

CD3D12GPUBuffer::CD3D12GPUBuffer(ID3D12Device* Device, uint32 InElementSize, uint32 InElementCount, D3D12_RESOURCE_FLAGS Flags /*= D3D12_RESOURCE_FLAG_NONE*/)
	: CD3D12Resource(InElementSize * InElementCount)
	, ElementSize(InElementSize)
	, ElementCount(InElementCount)
{
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(BufferSize, Flags);
	VERIFYD3D12RESULT(Device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(Resource.GetAddressOf())
	));
}