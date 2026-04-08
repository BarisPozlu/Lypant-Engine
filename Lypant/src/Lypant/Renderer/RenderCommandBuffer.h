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
		virtual bool BeginCommands() = 0;
		virtual void EndCommands() = 0;
		virtual void BeginImmediateCommands() = 0;
		virtual void EndImmediateCommands() = 0;
		virtual void BeginSubpass(const Subpass& subpass, bool IsImmediate = false) = 0;
		virtual void EndSubpass(const Subpass& subpass, bool IsImmediate = false) = 0;
		virtual void Draw(const Mesh& mesh, const std::shared_ptr<Shader>& shader, uint32_t instanceCount = 1, bool IsImmediate = false) = 0;
		virtual void Submit(const Mesh& mesh, const glm::mat4& modelMatrix, uint32_t instanceCount = 1) = 0;
		virtual void Execute(const std::shared_ptr<Shader>& shader) = 0;
		virtual void MergeMeshes() = 0;
		virtual void BindEnvironmentBuffer(const std::shared_ptr<Buffer>& buffer) = 0;
	};
}
