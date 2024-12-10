#pragma once

#include "VertexArray.h"
#include "RendererAPI.h"

namespace lypant
{
	class RenderCommand
	{
	public:
		inline static void SetClearColor(float r, float g, float b, float a) { RendererAPI::SetClearColor(r, g, b, a); }
		inline static void Clear() { RendererAPI::Clear(); }
		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) { RendererAPI::DrawIndexed(vertexArray); }
	};
}
