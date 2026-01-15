#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Image.h"
#include "Buffer.h"
#include "Shader.h"
#include "Pipeline.h"
#include "RenderTarget.h"

namespace lypant
{
	struct ImageBinding
	{
		std::shared_ptr<Image> Image;
		uint32_t Binding;
	};

	struct BufferBinding
	{
		std::shared_ptr<Buffer> Buffer;
		uint32_t Binding;
		uint32_t Offset;
		uint32_t Range;
	};

	struct DrawData
	{
		DrawData(const Mesh& mesh, const glm::mat4& modelMatrix, uint32_t instanceCount) : MeshData(mesh), ModelMatrix(modelMatrix), InstanceCount(instanceCount) { }
		Mesh MeshData;
		glm::mat4 ModelMatrix;
		uint32_t InstanceCount;
	};

	enum SubpassFlags
	{
		SubpassFlagNone = 0,
		SubpassFlagDrawWithMaterial = 1
	};

	class Subpass
	{
	public:
		virtual ~Subpass() = default;
		static std::unique_ptr<Subpass> Create(const std::shared_ptr<RenderTarget>& renderTarget, const RenderTargetOperation& op, const std::shared_ptr<Shader>& shader, const std::vector<ImageBinding>& dataBindings, uint32_t uniformBufferSize, const void* data, bool isDynamic = false, int subpassFlags = SubpassFlagNone);
		virtual void Submit(const Mesh& mesh, const glm::mat4& modelMatrix, uint32_t instanceCount = 1) = 0;
		virtual void ClearDrawData() = 0;
		virtual const std::vector<DrawData>& GetDrawData() const = 0;
		virtual const std::shared_ptr<Shader>& GetShader() const = 0;
		virtual int GetFlags() const = 0;
		// TODO: Setting the render target does not update the graphics pipeline, even though in some situtations it might be needed.
		virtual void SetRenderTarget(const std::shared_ptr<RenderTarget>& renderTarget) = 0;
		virtual void UploadData(const void* data, uint32_t size, uint32_t offset) = 0;
	};

	class RenderPass
	{
	public:
		virtual ~RenderPass() = default;
		static std::unique_ptr<RenderPass> Create(const std::vector<Subpass>& subpasses);
	};
}
