#pragma once

#include <memory>
#include "RenderPass.h"
#include "Mesh.h"

namespace lypant
{
	class RenderCommandBuffer
	{
	public:
		static RenderCommandBuffer* Create();
		RenderCommandBuffer() = default;
		virtual ~RenderCommandBuffer() = default;
		virtual void BeginCommands() = 0;
		virtual void EndCommands() = 0;
		virtual void BeginSubpass(const Subpass& subpass) = 0;
		virtual void EndSubpass(const Subpass& subpass) = 0;
		virtual void DrawMesh(const Mesh& mesh, const std::shared_ptr<Shader>& shader, uint32_t instanceCount = 1) = 0;
	};
}
