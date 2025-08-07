#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <Lypant/Renderer/RenderCommandBuffer.h>

// TODO: Remove
#include "VulkanShader.h"
#include "VulkanPipeline.h"
#include "VulkanBuffer.h"

namespace lypant
{
	class VulkanRenderCommandBuffer : public RenderCommandBuffer
	{
	public:
		VulkanRenderCommandBuffer();
		virtual ~VulkanRenderCommandBuffer();
		virtual void BeginCommands() override;
		virtual void EndCommands() override;
		// TODO: Remove
		void Test();
		//TODO: If you call this function twice in one frame it will cause problems
		virtual void SetRenderTargetToDefault() override;
		inline VkCommandBuffer GetCommandBuffer() { return GetCurrentFrame().CommandBuffer; }
	private:
		struct FrameData
		{
			VkCommandPool CommandPool;
			VkCommandBuffer CommandBuffer;
			VkSemaphore ImageReceivedSemaphore;
			VkFence FrameFinishedFence;
		};
		inline FrameData& GetCurrentFrame() { return m_FrameData[m_CurrentFrame]; }
		void CreateCommandResources();
		void CreateSyncResources();
		void DestroyCommandResources();
		void DestroySyncResources();
	private:
		inline static constexpr uint32_t s_MaxFramesInFlight = 2;
	private:
		std::array<FrameData, s_MaxFramesInFlight> m_FrameData;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		uint32_t m_CurrentFrame = 0;

		// TODO: Remove
		std::shared_ptr<VulkanShader> m_Shader;
		std::shared_ptr<VulkanGraphicsPipeline> m_Pipeline;
		std::shared_ptr<VulkanVertexBuffer> m_VertexBuffer;
		std::shared_ptr<VulkanIndexBuffer> m_IndexBuffer;
	};
}
