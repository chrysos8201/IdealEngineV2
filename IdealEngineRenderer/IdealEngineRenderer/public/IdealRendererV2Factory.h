#pragma once
#include <memory>
#include "IdealRendererV2Desc.h"

enum class EGraphicsInterfaceType
{
	D3D12,
	NONE,
};


namespace Ideal
{
	class IdealRendererV2;
}

std::shared_ptr<Ideal::IdealRendererV2> CreateRenderer(
	const IdealRendererV2Desc& Desc
);