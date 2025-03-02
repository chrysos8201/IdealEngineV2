#pragma once
#include "D3D12/D3D12Thirdparty.h"
#include "Core/Core.h"
#include <vector>
#include <memory>

class CD3D12Texture;

class CDeferredRenderPass
{
public:
	CDeferredRenderPass() {};
	~CDeferredRenderPass() {};

public:
	void Init();
	void SetFinalRenderTarget(std::shared_ptr<CD3D12Texture> InFinalRenderTarget);

private:
	std::vector<std::shared_ptr<CD3D12Texture>> MRTs;
};

