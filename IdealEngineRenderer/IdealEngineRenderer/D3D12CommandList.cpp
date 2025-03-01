#include "D3D12CommandList.h"
#include "D3D12Device.h"
#include "D3D12Util.h"

CD3D12CommandAllocator::CD3D12CommandAllocator(std::shared_ptr<CD3D12Device> Device, D3D12_COMMAND_LIST_TYPE InCommandListType)
	: CommandListType(InCommandListType)
{
	VERIFYD3D12RESULT(Device->GetDevice()->CreateCommandAllocator(CommandListType, IID_PPV_ARGS(CommandAllocator.GetAddressOf())));

}

CD3D12CommandAllocator::~CD3D12CommandAllocator()
{
	CommandAllocator->Release();
}

void CD3D12CommandAllocator::Reset()
{
	VERIFYD3D12RESULT(CommandAllocator->Reset());
}

CD3D12CommandList::CD3D12CommandList(std::shared_ptr<CD3D12Device> Device, std::shared_ptr<CD3D12CommandAllocator> CommandAllocator)
	: CommandListType(CommandAllocator->CommandListType)
{

	switch (CommandListType)
	{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			VERIFYD3D12RESULT(Device->GetDevice()->CreateCommandList(
				0, // GPU MASK
				CommandListType,
				CommandAllocator->GetCommandAllocator().Get(),
				nullptr,
				IID_PPV_ARGS(GraphicsCommandList9.GetAddressOf())
			));
			break;
		case D3D12_COMMAND_LIST_TYPE_COPY:
			VERIFYD3D12RESULT(Device->GetDevice()->CreateCommandList(
				0, // GPU MASK
				CommandListType,
				CommandAllocator->GetCommandAllocator().Get(),
				nullptr,
				IID_PPV_ARGS(CommandList.GetAddressOf())
			));
			break;
		default:
			return;
	}
	
}

void CD3D12CommandList::Reset(std::shared_ptr<CD3D12CommandAllocator> CommandAllocator)
{
	if (IsClosed())
	{
		VERIFYD3D12RESULT(GraphicsCommandList9->Reset(CommandAllocator->GetCommandAllocator().Get(), nullptr));
	}
}

void CD3D12CommandList::Close()
{
	if (IsOpen())
	{
		VERIFYD3D12RESULT(GraphicsCommandList9->Close());
		bClosed = true;
	}
}
