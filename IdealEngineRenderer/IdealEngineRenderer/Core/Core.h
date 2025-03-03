#pragma once

#include "Core/Types.h"
#include "Misc/Utils/SimpleMath.h"

#include <memory>
#include <windows.h>
#include <cassert>

#pragma comment(lib, "../Libraries/DebugLib/assimp/assimp-vc143-mtd.lib")

using namespace DirectX::SimpleMath;

inline void MyMessageBoxW(const wchar_t* Message)
{
	if (Message == nullptr)
	{
		return;
	}
	MessageBox(NULL, Message, L"Failed", MB_OK);
	assert(false);
}

inline void MyMessageBox(const char* Message)
{
	if (Message == nullptr)
	{
		return;
	}
	MessageBoxA(NULL, Message, "Failed", MB_OK);
	assert(false);
}