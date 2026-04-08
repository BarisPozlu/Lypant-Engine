#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <Lypant/Renderer/RenderCommandBuffer.h>
#include <Lypant/Renderer/RenderPass.h>
#include "VulkanGraphicsContext.h"
#include "VulkanCommandBuffer.h"
#include "Lypant/Renderer/EnvironmentBufferLayout.h"

namespace lypant
{
	class VulkanDescriptorSet;
	class VulkanBuffer;

	class VulkanRenderCommandBuffer : public RenderCommandBuffer
	{
	public:
		VulkanRenderCommandBuffer();
		virtual ~VulkanRenderCommandBuffer();
		// Return false If could not begin commands successfully
		virtual bool BeginCommands() override;
		virtual void EndCommands() override;
		inline virtual void BeginImmediateCommands() override { m_ImmediateCommandBuffer.BeginCommands(); }
		inline virtual void EndImmediateCommands() override { m_ImmediateCommandBuffer.EndCommands(); }
		virtual void BeginSubpass(const Subpass& subpass, bool IsImmediate = false) override;
		virtual void EndSubpass(const Subpass& subpass, bool IsImmediate = false) override;
		virtual void Draw(const Mesh& mesh, const std::shared_ptr<Shader>& shader, uint32_t instanceCount = 1, bool IsImmediate = false) override;
		virtual void Submit(const Mesh& mesh, const glm::mat4& modelMatrix, uint32_t instanceCount = 1) override;
		virtual void Execute(const std::shared_ptr<Shader>& shader) override;
		virtual void MergeMeshes() override;
		virtual void BindEnvironmentBuffer(const std::shared_ptr<Buffer>& buffer) override;

		inline VkCommandBuffer GetCommandBuffer() { return GetCurrentFrame().CommandBuffer; }
	private:
		struct FrameData
		{
			VkCommandPool CommandPool;
			VkCommandBuffer CommandBuffer;
			VkSemaphore ImageReceivedSemaphore;
			VkFence FrameFinishedFence;
		};
		inline FrameData& GetCurrentFrame() { return m_FrameData[VulkanGraphicsContext::Get().GetCurrentFrameIndex()]; }
		void CreateCommandResources();
		void DestroyCommandResources();
	private:
		std::array<FrameData, VulkanGraphicsContext::s_MaxFramesInFlight> m_FrameData;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		VulkanImmediateCommandBuffer m_ImmediateCommandBuffer;
		std::unique_ptr<VulkanDescriptorSet> m_EnvironmentDescriptorSet;
		std::array<uint32_t, EnvironmentBufferLayout::s_BindingCount> m_DynamicOffsets;
		uint32_t m_EnvironmentBufferSize = 0;

		// NOTE: Draw data will not be stored here after dynamic loading
		struct DrawData
		{
			DrawData(const Mesh& mesh, const glm::mat4& modelMatrix, uint32_t instanceCount) : MeshData(mesh), ModelMatrix(modelMatrix), InstanceCount(instanceCount) {}
			Mesh MeshData;
			glm::mat4 ModelMatrix;
			uint32_t InstanceCount;
		};

		std::vector<DrawData> m_DrawData;
		std::unique_ptr<VulkanDescriptorSet> m_IndirectDescriptorSet;
		std::shared_ptr<VulkanBuffer> m_VertexBuffer;
		std::shared_ptr<VulkanBuffer> m_IndexBuffer;
		std::shared_ptr<VulkanBuffer> m_MetaBuffer;
		std::shared_ptr<VulkanBuffer> m_MatrixBuffer;
		std::shared_ptr<VulkanBuffer> m_MaterialBuffer;
		std::shared_ptr<VulkanBuffer> m_IndirectBuffer;
	};
}
