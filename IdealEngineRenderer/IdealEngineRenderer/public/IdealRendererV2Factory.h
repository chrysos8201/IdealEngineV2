#pragma once
#include <memory>
#include "IdealRendererV2Desc.h"


#ifdef CREATEDLL_EXPORTS
#define IDEAL_DLL __declspec(dllexport)
#else
#define IDEAL_DLL __declspec(dllimport)
#endif

enum class EGraphicsInterfaceType
{
	D3D12,
	NONE,
};


namespace Ideal
{
	class IdealRendererV2;
}

extern "C" IDEAL_DLL void __stdcall CreateRenderer2(std::shared_ptr<Ideal::IdealRendererV2>& OutRenderer, const IdealRendererV2Desc& Desc);
extern "C" IDEAL_DLL void __stdcall DestroyRenderer(std::shared_ptr<Ideal::IdealRendererV2>& Renderer);

