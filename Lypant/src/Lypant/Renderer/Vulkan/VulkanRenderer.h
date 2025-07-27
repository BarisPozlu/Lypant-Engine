#pragma once

#include <vulkan/vulkan.h>
#include <Lypant/Renderer/Renderer.h>

// I am thinking of having a scene renderer which takes the scene data from the scene
// which is what the "old renderer" was doing and then uses the platform agnostic renderer which will only have a vulkan renderer for now
// to render objects with materials etc. and this vulkan renderer is free to just upload all of the commands required into a command buffer
// after all of that the platform agnostic renderer will have Renderer::BeginRendering() and Renderer::EndRendering() this way we are able to use the queue as we please

// IMP DETAILS: have the vulkan renderer derive from the renderer and have the renderer be a singleton that way use just static functions that call the
// pure virtual ones inside the renderer

namespace lypant
{
	//TODO: protected?
	class VulkanRenderer : public Renderer
	{
	public:
		VulkanRenderer();
		virtual ~VulkanRenderer();
		virtual void BeginRendering_Impl() override;
		virtual void EndRendering_Impl() override;
		virtual void ClearImage_Impl() override;
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
		uint32_t m_CurrentFrame = 0;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
	};
}
