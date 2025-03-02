#pragma once
#include <windows.h>
struct IdealRendererV2Desc
{
	IdealRendererV2Desc()
		: Hwnd(nullptr)
		, Width(1080)
		, Height(720)
		, bDebugMode(false)
		, bTearingSupport(true)
	{
	}
	void* Hwnd;
	int Width;
	int Height;
	bool bDebugMode;
	bool bTearingSupport;
};
