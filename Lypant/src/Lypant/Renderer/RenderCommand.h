#pragma once

#include "VertexArray.h"
#include "RendererAPI.h"

namespace lypant
{
	class RenderCommand
	{
	public:
		inline static void Init() { RendererAPI::Init(); }
		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) { RendererAPI::SetViewport(x, y, width, height); }
		inline static void SetClearColor(float r, float g, float b, float a) { RendererAPI::SetClearColor(r, g, b, a); }
		inline static void Clear() { RendererAPI::Clear(); }
		inline static void SetDepthTest(bool value) { RendererAPI::SetDepthTest(value); }
		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) { RendererAPI::DrawIndexed(vertexArray); }
	};
}
