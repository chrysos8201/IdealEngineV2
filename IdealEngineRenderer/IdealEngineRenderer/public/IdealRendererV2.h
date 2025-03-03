#pragma once
#include <string>

namespace Ideal
{
	class IdealRendererV2
	{
	public:
		IdealRendererV2() {}
		virtual ~IdealRendererV2() = default;

	public:
		virtual void Init() abstract;
		virtual void Render() abstract;
		virtual void Destroy() abstract;

	public:
		virtual void LoadStaticMeshObject(const std::wstring& Path) abstract;
	};

}