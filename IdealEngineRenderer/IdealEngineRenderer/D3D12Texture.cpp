#include "D3D12Texture.h"

CD3D12Texture::CD3D12Texture(ComPtr<ID3D12Resource> InResource)
	: CD3D12Resource(InResource)
{

}
