#include "D3D12/D3D12Renderer.h"
#include "D3D12/D3D12CommandList.h"
#include "D3D12/D3D12Device.h"
#include "D3D12/D3D12Util.h"
#include "D3D12/D3D12Descriptors.h"
#include "D3D12/D3D12Texture.h"
#include "D3D12/D3D12Viewport.h"

CD3D12Renderer::CD3D12Renderer(const IdealRendererV2Desc& InRenderDesc)
	: RendererDesc(InRenderDesc)
{
	Init();
}

void CD3D12Renderer::Init()
{
	bool bDebugMode = true;
	Device = std::make_shared<CD3D12Device>(bDebugMode);
	Device->Init();

	//-------Viewport-------//
	Viewport = std::make_shared<CD3D12Viewport>(RendererDesc.Width, RendererDesc.Height);

	//-------Swap Chain-------//
	CreateSwapChains();


}

void CD3D12Renderer::Render()
{
	// Begin Render
	uint64 currentContextIndex = Device->GetCurrentContextIndex();
	std::shared_ptr<CD3D12CommandList> commandList = Device->GetCommandContext().CommandList;
	std::shared_ptr<CD3D12Texture> currentSwapChainTexture = SwapChainTextures[currentContextIndex];
	Device->GetCommandContext().TransitionResource(currentSwapChainTexture, D3D12_RESOURCE_STATE_RENDER_TARGET);
	float black[4] = { 0,0,0,1 };
	commandList->GetGraphicsCommandList()->ClearRenderTargetView(currentSwapChainTexture->GetRTV().GetCPUDescriptorHandleStart(), black, 0, nullptr);
	commandList->GetGraphicsCommandList()->RSSetViewports(1, &Viewport->GetViewport());
	commandList->GetGraphicsCommandList()->RSSetScissorRects(1, &Viewport->GetScissorRect());

	auto swapChainRTV = currentSwapChainTexture->GetRTV();
	auto swapChainCpuHandle = swapChainRTV.GetCPUDescriptorHandleStart();
	commandList->GetGraphicsCommandList()->OMSetRenderTargets(1, &swapChainCpuHandle, FALSE, nullptr);
	
	// End Render
	Device->GetCommandContext().TransitionResource(currentSwapChainTexture, D3D12_RESOURCE_STATE_PRESENT);
	Device->GetCommandContext().CommandList->Close();
	ID3D12CommandList* ppCommandLists[] = { Device->GetCommandContext().CommandList->GetGraphicsCommandList().Get() };
	Device->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Present
	Device->Fence();
	uint32 SyncInterval = 0;
	uint32 PresentFlags = 0;
	PresentFlags = DXGI_PRESENT_ALLOW_TEARING;

	VERIFYD3D12RESULT(SwapChain->Present(0, PresentFlags));

	FrameIndex = SwapChain->GetCurrentBackBufferIndex();
	Device->MoveToNextContextIndex();

}

void CD3D12Renderer::Destroy()
{
	// Destroy
	Device->Fence();
	for (uint32 i = 0; i < MAX_PENDING_FRAME_COUNT; i++)
	{
		Device->MoveToNextContextIndex();
	}

	for (int i = 0; i < MAX_PENDING_FRAME_COUNT; i++)
	{
		SwapChainTextures[i]->Destroy();
	}
	SwapChainTextures.clear();

	Device->Destroy();
}

void CD3D12Renderer::CreateSwapChains()
{
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = SWAP_CHAIN_FRAME_COUNT;
	swapChainDesc.Width = RendererDesc.Width;
	swapChainDesc.Height = RendererDesc.Height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.Flags = RendererDesc.bTearingSupport ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

	ComPtr<IDXGISwapChain1> swapChain;
	VERIFYD3D12RESULT(Device->GetDXGIFactory7()->CreateSwapChainForHwnd(
		Device->GetCommandQueue().Get(),
		(HWND)RendererDesc.Hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		swapChain.GetAddressOf()
	));

	VERIFYD3D12RESULT(Device->GetDXGIFactory7()->MakeWindowAssociation(
		(HWND)RendererDesc.Hwnd,
		DXGI_MWA_NO_ALT_ENTER
	));

	VERIFYD3D12RESULT(swapChain.As(&SwapChain););
	FrameIndex = SwapChain->GetCurrentBackBufferIndex();

	// Set Texture
	SwapChainTextures.resize(SWAP_CHAIN_FRAME_COUNT);
	for (uint32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; i++)
	{
		ComPtr<ID3D12Resource> resource;
		VERIFYD3D12RESULT(SwapChain->GetBuffer(i, IID_PPV_ARGS(resource.GetAddressOf())));

		// RTV를 만든다.
		auto handle = Device->GetRTVHeap()->Allocate(1);
		Device->GetDevice()->CreateRenderTargetView(resource.Get(), nullptr, handle.GetCPUDescriptorHandleStart());
		SwapChainTextures[i] = std::make_shared<CD3D12Texture>(resource);
		SwapChainTextures[i]->EmplaceRTV(handle);
	}
}
