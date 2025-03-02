#pragma once
#include "Core/Core.h"
#include "D3D12/D3D12Thirdparty.h"
#include <vector>

// �ӽ� dx12, ������ �ٸ� api�� ���ٸ� �ٲٰڴ�.
class CD3DShader
{
public:
	void AddShaderData(const std::vector<uint8>& InByteCode);
	const CD3DX12_SHADER_BYTECODE& GetShaderByteCode() const { return ShaderByteCode; }

private:
	CD3DX12_SHADER_BYTECODE ShaderByteCode;
	std::vector<uint8> ShaderData;
};

