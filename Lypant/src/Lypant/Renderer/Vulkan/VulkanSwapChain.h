#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "VulkanImage.h"

namespace lypant
{
	struct VulkanDeviceSurfaceDetails;

	//TODO: Give option to use vsync, adaptive vsync or none of them
	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(VkDevice device, VkSurfaceKHR surface, const VulkanDeviceSurfaceDetails& deviceSurfaceDetails);
		~VulkanSwapChain();
		// Acquires the next Image from the swap chain
		VulkanImage2D& GetNextImage(VkSemaphore signalSemaphore);
		inline uint32_t GetCurrentImageIndex() const { return m_ImageIndex; }
		inline uint32_t GetImageCount() const { return m_Images.size(); }	
	private:
		VkSwapchainKHR m_SwapChain;
		std::vector<VulkanImage2D> m_Images;
		uint32_t m_ImageIndex = UINT32_MAX;
	};
}
