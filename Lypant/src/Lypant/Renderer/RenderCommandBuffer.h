#pragma once

#include <memory>

namespace lypant
{
	class RenderCommandBuffer
	{
	public:
		static std::unique_ptr<RenderCommandBuffer> Create();
		virtual ~RenderCommandBuffer() = default;
		RenderCommandBuffer() = default;
		virtual void BeginCommands() = 0;
		virtual void EndCommands() = 0;
		virtual void SetRenderTargetToDefault() = 0;
	};
}
