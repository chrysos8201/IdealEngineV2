#pragma once

#include "Core/Types.h"

#include <memory>
#include <windows.h>
#include <cassert>


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