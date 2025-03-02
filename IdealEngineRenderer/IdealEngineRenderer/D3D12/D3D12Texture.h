#pragma once
#include "Core/Core.h"
#include "D3D12/D3D12Resource.h"

struct ID3D12Device;

class CD3D12Texture : public CD3D12Resource
{
public:
	CD3D12Texture(ComPtr<ID3D12Resource> InResource);
	virtual ~CD3D12Texture() {};

private:
	uint32 Width;
	uint32 Height;
};

