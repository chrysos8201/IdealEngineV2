#pragma once
#include "Types.h"


class CIndexCreator
{
public:
	CIndexCreator();
	~CIndexCreator();

public:
	void Init(uint32 MaxNum);
	int32 Allocate();
	void Free(uint32 Index);

private:
	uint32* m_table = nullptr;
	uint32 m_allocatedCount = 0;
	uint32 m_maxNum;
};

