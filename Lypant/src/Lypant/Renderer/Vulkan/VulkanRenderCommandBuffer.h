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
		virtual void DrawMesh(const Mesh& mesh, const std::shared_ptr<Shader>& shader, uint32_t instanceCount = 1) override;

		inline VkCommandBuffer GetCommandBuffer() { return GetCurrentFrame().CommandBuffer; }
		inline constexpr uint32_t GetMaxFramesInFlight() const { return s_MaxFramesInFlight; }
		inline uint32_t GetCurrentFrameIndex() const { return m_CurrentFrameIndex; }
	private:
		struct FrameData
		{
			VkCommandPool CommandPool;
			VkCommandBuffer CommandBuffer;
			VkSemaphore ImageReceivedSemaphore;
			VkFence FrameFinishedFence;
		};
		inline FrameData& GetCurrentFrame() { return m_FrameData[m_CurrentFrameIndex]; }
		void CreateCommandResources();
		void CreateSyncResources();
		void DestroyCommandResources();
		void DestroySyncResources();
	private:
		inline static constexpr uint32_t s_MaxFramesInFlight = 2;
	private:
		std::array<FrameData, s_MaxFramesInFlight> m_FrameData;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		uint32_t m_CurrentFrameIndex = 0;
	};
}
