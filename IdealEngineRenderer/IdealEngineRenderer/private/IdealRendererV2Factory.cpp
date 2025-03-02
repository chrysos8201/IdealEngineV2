#include "IdealRendererV2Factory.h"
#include "D3D12/D3D12Renderer.h"
std::shared_ptr<Ideal::IdealRendererV2> CreateRenderer(const IdealRendererV2Desc& Desc)
{
	return std::make_shared<CD3D12Renderer>(Desc);
}

