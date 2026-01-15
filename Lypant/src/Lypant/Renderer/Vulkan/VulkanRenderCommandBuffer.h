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
		virtual void ExecuteSubpass(Subpass& subpass, bool IsImmediate = false) override;
		virtual void DrawMesh(const Mesh& mesh, const std::shared_ptr<Shader>& shader, const glm::mat4& modelMatrix, uint32_t instanceCount = 1, bool IsImmediate = false) override;
		virtual void DrawMeshWithMaterial(const Mesh& mesh, const std::shared_ptr<Shader>& shader, const glm::mat4& modelMatrix, uint32_t instanceCount = 1, bool IsImmediate = false) override;
		virtual void PushData(const void* data, uint32_t size, const std::shared_ptr<Shader>& shader, int shaderStageFlags, bool IsImmediate = false) override;
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
		std::unique_ptr<class VulkanDescriptorSet> m_EnvironmentDescriptorSet;
		std::array<uint32_t, EnvironmentBufferLayout::s_BindingCount> m_DynamicOffsets;
		uint32_t m_EnvironmentBufferSize = 0;
	};
}
