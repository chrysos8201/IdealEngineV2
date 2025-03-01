#pragma once

#define G_SWAP_CHAIN_NUM 4


#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#define FAILED(hr) (((HRESULT)(hr)) < 0)

inline void Check(HRESULT hr, const wchar_t* Message)
{
	if (FAILED(hr))
	{
		if (Message == nullptr)
		{
			return;
		}
		MessageBox(NULL, Message, L"Check", MB_OK);
		//assert(false);
	}
}