#pragma once
#include "Core/Core.h"
#include "D3D12/D3D12Thirdparty.h"
#include <dxcapi.h>
#include <string>

class CD3DShader;

class CDXCManager
{
public:
	CDXCManager();
	~CDXCManager() = default;

	void CompileShader(const std::wstring& FilePath, const std::wstring& ShaderModel, ComPtr<IDxcBlob>& OutBlob);
	void CompileShader(const std::wstring& FilePath, const std::wstring& ShaderModel, const std::wstring& EntryPoint, ComPtr<IDxcBlob>& OutBlob);
	void CompileShaderAndSave(const std::wstring& FilePath, const std::wstring& SavePath, const std::wstring& SaveName,
		const std::wstring& ShaderModel, ComPtr<IDxcBlob>& OutBlob, const std::wstring& EntryPoint = L"", bool HasEntry = false
	);
	void LoadShaderFile(const std::wstring& FilePath, std::shared_ptr<CD3DShader>& OutShader);
	void AddIncludeDirectories(const std::wstring& IncludeDir) { m_includeDirectories.push_back(IncludeDir); }

private:
	void ReadShaderFile(const std::wstring& FilePath, DxcBuffer* OutSourceBuffer, std::vector<char>& SourceData);

	void AddToArgumentsIncludeDirectories(std::vector<LPCWSTR>& Args);
	void ErrorCheck(ComPtr<IDxcResult> Result);
	void OutPDB(ComPtr<IDxcResult> Result, const std::wstring& Path);


private:
	std::vector<std::wstring> m_includeDirectories;

private:
	ComPtr<IDxcCompiler3> Compiler;
	ComPtr<IDxcUtils> Utils;
	ComPtr<IDxcIncludeHandler> IncludeHandler;
};

