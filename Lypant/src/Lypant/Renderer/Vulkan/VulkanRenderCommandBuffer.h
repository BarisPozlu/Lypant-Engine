#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <Lypant/Renderer/RenderCommandBuffer.h>
#include <Lypant/Renderer/RenderPass.h>

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

		virtual void BeginSubpass(const Subpass& subpass) override;
		virtual void EndSubpass(const Subpass& subpass) override;

		// TODO: Remove
		void Test();
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

		//TODO: Remove
		std::shared_ptr<VulkanVertexBuffer> m_VertexBuffer;
		std::shared_ptr<VulkanIndexBuffer> m_IndexBuffer;
	private:
		// NOTE: Default render target needs to acquire the image from the swap chain and has to signal a semaphore that is stored in this class
		friend class VulkanDefaultRenderTarget;
	};
}
