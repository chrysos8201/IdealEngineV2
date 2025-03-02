#pragma once
#include "Core/Types.h"
#include "D3D12/D3D12Thirdparty.h"
#include "D3D12/D3D12Definition.h"
#include "Core/Core.h"
#include <string>
#include <memory>

class CD3D12Device;

extern void VerifyD3D12Result(HRESULT D3DResult, const ANSICHAR* Code, const ANSICHAR* FileName, uint32 Line, std::shared_ptr<CD3D12Device> Device);

#define VERIFYD3D12RESULT_EX(x, Device)	{HRESULT hres = x; if (FAILED(hres)) { VerifyD3D12Result(hres, #x, __FILE__, __LINE__, Device); }}
#define VERIFYD3D12RESULT(x)			{HRESULT hres = x; if (FAILED(hres)) { VerifyD3D12Result(hres, #x, __FILE__, __LINE__, nullptr); }}
