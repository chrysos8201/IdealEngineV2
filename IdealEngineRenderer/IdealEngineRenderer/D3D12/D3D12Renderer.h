#pragma once
#include "IdealRendererV2.h"
#include "IdealRendererV2Desc.h"

#include "Core/Core.h"
#include "D3D12/D3D12Thirdparty.h"
#include "D3D12/D3D12Definition.h"

class CD3D12Device;
class CD3D12CommandAllocator;
class CD3D12CommandList;
class CD3D12DescriptorHeap;
class CD3D12Texture;
class CD3D12Viewport;

class CD3D12Renderer : public Ideal::IdealRendererV2
{
	static const uint32 SWAP_CHAIN_FRAME_COUNT = G_SWAP_CHAIN_NUM;
	static const uint32 MAX_PENDING_FRAME_COUNT = SWAP_CHAIN_FRAME_COUNT - 1;

public:
	CD3D12Renderer(const IdealRendererV2Desc& InRenderDesc);
	virtual ~CD3D12Renderer() {}

	virtual void Init() override;
	virtual void Render() override;
	virtual void Destroy() override;

public:
	virtual void LoadStaticMeshObject(const std::wstring& Path) override;

private:
	void CreateSwapChains();

private:
	IdealRendererV2Desc RendererDesc;

	std::shared_ptr<CD3D12Device> Device;
	ComPtr<IDXGISwapChain4> SwapChain = nullptr;
	uint32 FrameIndex = 0;
	std::vector<std::shared_ptr<CD3D12Texture>> SwapChainTextures;
	std::shared_ptr<CD3D12Viewport> Viewport;
};