#pragma once
#include "D3D12/D3D12Thirdparty.h"
#include "Core/Core.h"

class CD3D12Viewport
{
public:
	CD3D12Viewport();
	CD3D12Viewport(uint32 InWidth, uint32 InHeight);

	void Resize(uint32 InWidth, uint32 InHeight);

	const CD3DX12_VIEWPORT& GetViewport() const { return Viewport; }
	const CD3DX12_RECT& GetScissorRect() const { return ScissorRect; }
	
private:
	uint32 Width;
	uint32 Height;
	CD3DX12_VIEWPORT Viewport;
	CD3DX12_RECT ScissorRect;
};

