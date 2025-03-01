#pragma once
#include "Core.h"
#include "D3D12Thirdparty.h"
#include "D3D12Definition.h"

class CD3D12Device;
class CD3D12CommandAllocator;
class CD3D12CommandList;
class CD3D12DescriptorHeap;
class CD3D12Texture;
class CD3D12Viewport;

struct D3D12RendererDesc
{
	D3D12RendererDesc() 
		: Hwnd(NULL)
		, Width(1080)
		, Height(720)
		, bDebugMode(false)
		, bTearingSupport(true)
	{}
	HWND Hwnd;
	uint32 Width;
	uint32 Height;
	bool bDebugMode;
	bool bTearingSupport;
};

class CD3D12Renderer
{
	static const uint32 SWAP_CHAIN_FRAME_COUNT = G_SWAP_CHAIN_NUM;
	static const uint32 MAX_PENDING_FRAME_COUNT = SWAP_CHAIN_FRAME_COUNT - 1;

public:
	CD3D12Renderer(const D3D12RendererDesc& InRenderDesc);
	virtual ~CD3D12Renderer() {}

	void Init();
	void Render();

private:
	void CreateSwapChains();

private:
	D3D12RendererDesc RendererDesc;

	std::shared_ptr<CD3D12Device> Device;
	ComPtr<IDXGISwapChain4> SwapChain = nullptr;
	uint32 FrameIndex = 0;
	std::vector<std::shared_ptr<CD3D12Texture>> SwapChainTextures;
	std::shared_ptr<CD3D12Viewport> Viewport;
};

