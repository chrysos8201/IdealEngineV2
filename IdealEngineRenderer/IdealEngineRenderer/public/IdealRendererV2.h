#pragma once

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
	};
}