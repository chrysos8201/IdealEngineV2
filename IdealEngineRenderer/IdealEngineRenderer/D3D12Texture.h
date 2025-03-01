#pragma once
#include "D3D12Resource.h"
#include "Core.h"

struct ID3D12Device;

class CD3D12Texture : public CD3D12Resource
{
public:
	CD3D12Texture(ComPtr<ID3D12Resource> InResource);

private:
	uint32 Width;
	uint32 Height;
};

