#pragma once

#include <vulkan/vulkan.h>

namespace lypant
{
	// Will block the cpu until its commands are done.
	class VulkanImmediateCommandBuffer
	{
	public:
		VulkanImmediateCommandBuffer();
		~VulkanImmediateCommandBuffer();
		void BeginCommands();
		void EndCommands();
		inline VkCommandBuffer GetVkCommandBuffer() { return m_CommandBuffer; }
	private:
		static void Init(VkDevice device, uint32_t graphicsQueueFamilyIndex);
		static void Shutdown();
	private:
		inline static VkCommandPool s_CommandPool = VK_NULL_HANDLE;
	private:
		VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
		VkFence m_Fence = VK_NULL_HANDLE;
	private:
		friend class VulkanGraphicsContext;
	};
}