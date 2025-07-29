#pragma once

#include <vulkan/vulkan.h>

namespace lypant
{
	// will block the cpu until its commands are done for now
	class VulkanImmediateCommandScope
	{
	public:
		VulkanImmediateCommandScope();
		~VulkanImmediateCommandScope();
		inline VkCommandBuffer GetCommandBuffer() { return s_CommandBuffer; }
	private:
		static void Init();
		static void Shutdown();
	private:
		inline static VkCommandPool s_CommandPool = VK_NULL_HANDLE;
		inline static VkCommandBuffer s_CommandBuffer = VK_NULL_HANDLE;
		inline static VkFence s_Fence = VK_NULL_HANDLE;
	private:
		friend class VulkanGraphicsContext;
	};
}