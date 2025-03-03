#pragma once
#include <string>

namespace StringUtils
{
	std::wstring ConvertStringToWString(const std::string& str);
	std::string ConvertWStringToString(const std::wstring& wstr);
}