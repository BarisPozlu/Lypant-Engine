#pragma once

#include <memory>
#include "RenderCommandBuffer.h"

namespace lypant
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void BeginRendering();
		static void EndRendering();
		// TODO: This is needed so that ImGui can record to its command buffer, might want to find another way later
		static RenderCommandBuffer& GetRenderCommandBuffer();
	private:
		struct RendererData
		{
			std::unique_ptr<RenderCommandBuffer> Cmd;
		};

		static RendererData* s_Data;
	};
}
