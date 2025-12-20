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
		virtual void BeginImmediateCommands() = 0;
		virtual void EndImmediateCommands() = 0;
		virtual void ExecuteSubpass(Subpass& subpass, bool IsImmediate = false) = 0;
		virtual void BeginSubpass(const Subpass& subpass, bool IsImmediate = false) = 0;
		virtual void EndSubpass(const Subpass& subpass, bool IsImmediate = false) = 0;
		virtual void DrawMesh(const Mesh& mesh, const std::shared_ptr<Shader>& shader, const glm::mat4& modelMatrix, uint32_t instanceCount = 1, bool IsImmediate = false) = 0;
		virtual void DrawMeshWithMaterial(const Mesh& mesh, const std::shared_ptr<Shader>& shader, const glm::mat4& modelMatrix, uint32_t instanceCount = 1, bool IsImmediate = false) = 0;
		virtual void PushData(const void* data, uint32_t size, const std::shared_ptr<Shader>& shader, int shaderStageFlags, bool IsImmediate = false) = 0;
		virtual void BindEnvironmentBuffer(const std::shared_ptr<Buffer>& buffer) = 0;
	};
}
