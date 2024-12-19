#pragma once

#include "lypch.h"
#include "Lypant/Renderer/VertexArray.h"

namespace lypant
{
	class RendererAPI
	{
	public:
		static void Init();
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		static void SetClearColor(float r, float g, float b, float a);
		static void Clear();
		static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray);
	};
}
