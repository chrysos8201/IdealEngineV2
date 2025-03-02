#include "Graphics/Shader/D3DShader.h"

void CD3DShader::AddShaderData(const std::vector<uint8>& InByteCode)
{
	ShaderData.insert(ShaderData.end(), InByteCode.begin(), InByteCode.end());
	ShaderByteCode = CD3DX12_SHADER_BYTECODE(ShaderData.data(), ShaderData.size());
}
