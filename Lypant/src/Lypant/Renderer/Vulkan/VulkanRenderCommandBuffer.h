#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <Lypant/Renderer/RenderCommandBuffer.h>
#include <Lypant/Renderer/RenderPass.h>
#include "VulkanGraphicsContext.h"
#include "VulkanCommandBuffer.h"

namespace lypant
{
	class VulkanRenderCommandBuffer : public RenderCommandBuffer
	{
	public:
		VulkanRenderCommandBuffer();
		virtual ~VulkanRenderCommandBuffer();
		virtual void BeginCommands() override;
		virtual void EndCommands() override;
		inline virtual void BeginImmediateCommands() override { m_ImmediateCommandBuffer.BeginCommands(); }
		inline virtual void EndImmediateCommands() override { m_ImmediateCommandBuffer.EndCommands(); }
		virtual void BeginSubpass(const Subpass& subpass, bool IsImmediate = false) override;
		virtual void EndSubpass(const Subpass& subpass, bool IsImmediate = false) override;
		virtual void DrawMesh(const Mesh& mesh, const std::shared_ptr<Shader>& shader, uint32_t instanceCount = 1, bool IsImmediate = false) override;
		virtual void BindEnvironmentBuffer(const std::shared_ptr<UniformBuffer>& buffer) override;

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
		uint32_t m_EnvironmentBufferSize = 0;
	};
}
