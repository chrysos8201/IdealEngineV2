#pragma once
#include "D3D12/D3D12Thirdparty.h"
#include <memory>

class CD3D12Device;

class CD3D12CommandAllocator
{
public:
	CD3D12CommandAllocator(std::shared_ptr<CD3D12Device> Device, D3D12_COMMAND_LIST_TYPE InCommandListType);
	~CD3D12CommandAllocator();

	void Reset();
	D3D12_COMMAND_LIST_TYPE CommandListType;

	ComPtr<ID3D12CommandAllocator> GetCommandAllocator() { return CommandAllocator; }

private:
	ComPtr<ID3D12CommandAllocator> CommandAllocator;
};

class CD3D12CommandList
{
private:
	CD3D12CommandList(CD3D12CommandList const&) = delete;
	CD3D12CommandList(CD3D12CommandList&&) = delete;

public:
	CD3D12CommandList(std::shared_ptr<CD3D12Device> Device, std::shared_ptr<CD3D12CommandAllocator> CommandAllocator);

	ComPtr<ID3D12GraphicsCommandList> GetGraphicsCommandList() { return GraphicsCommandList4; }

	void Reset(std::shared_ptr<CD3D12CommandAllocator> CommandAllocator);
	void Close();

	bool IsOpen() const { return !bClosed; }
	bool IsClosed() const { return bClosed; }

	D3D12_COMMAND_LIST_TYPE CommandListType;

private:
	ComPtr<ID3D12CommandList> CommandList;
	ComPtr<ID3D12GraphicsCommandList4> GraphicsCommandList4;

	bool bClosed = false;
};