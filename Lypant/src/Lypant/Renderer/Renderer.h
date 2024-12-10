#pragma once

#include "VertexArray.h"
#include "RenderCommand.h"

namespace lypant
{
	class Renderer
	{
	public:
		static void BeginScene();
		static void EndScene();

		static void Submit(const std::shared_ptr<VertexArray>& vertexArray);
	};
}
