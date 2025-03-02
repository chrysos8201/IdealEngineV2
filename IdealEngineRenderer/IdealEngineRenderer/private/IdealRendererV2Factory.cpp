#include "IdealRendererV2Factory.h"
#include "D3D12/D3D12Renderer.h"

void CreateRenderer2(std::shared_ptr<Ideal::IdealRendererV2>& OutRenderer, const IdealRendererV2Desc& Desc)
{
	OutRenderer = std::make_shared<CD3D12Renderer>(Desc);
}

void DestroyRenderer(std::shared_ptr<Ideal::IdealRendererV2>& Renderer)
{
	Renderer.reset();
}
