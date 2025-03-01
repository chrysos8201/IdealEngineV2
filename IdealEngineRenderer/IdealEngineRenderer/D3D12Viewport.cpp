#include "D3D12Viewport.h"

CD3D12Viewport::CD3D12Viewport()
	: Width(0)
	, Height(0)
	, Viewport(0.f, 0.f, 0.f, 0.f)
	, ScissorRect(0, 0, 0, 0)
{

}

CD3D12Viewport::CD3D12Viewport(uint32 InWidth, uint32 InHeight)
	: Width(InWidth)
	, Height(InHeight)
	, Viewport(0.f, 0.f, static_cast<float>(InWidth), static_cast<float>(InHeight))
	, ScissorRect(0, 0, static_cast<uint32>(InWidth), static_cast<uint32>(InHeight))
{

}

void CD3D12Viewport::Resize(uint32 InWidth, uint32 InHeight)
{
	Viewport = CD3DX12_VIEWPORT(0.f, 0.f, static_cast<float>(InWidth), static_cast<float>(InHeight));
	ScissorRect = CD3DX12_RECT(0, 0, static_cast<uint32>(InWidth), static_cast<uint32>(InHeight));
	Width = InWidth;
	Height = InHeight;
}
