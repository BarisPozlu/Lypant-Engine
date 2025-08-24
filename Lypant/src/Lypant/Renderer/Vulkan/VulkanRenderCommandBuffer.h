#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <Lypant/Renderer/RenderCommandBuffer.h>
#include <Lypant/Renderer/RenderPass.h>

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
		virtual void DrawMesh(const Mesh& mesh, const std::shared_ptr<Shader>& shader) override;

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
	private:
		// NOTE: Default render target needs to acquire the image from the swap chain and has to signal a semaphore that is stored in this class
		friend class VulkanDefaultRenderTarget;
	};
}
