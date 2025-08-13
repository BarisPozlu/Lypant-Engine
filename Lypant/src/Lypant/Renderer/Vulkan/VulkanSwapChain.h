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
		inline VkSwapchainKHR GetSwapChain() const { return m_SwapChain; }
		// Acquires the next Image from the swap chain.
		const std::shared_ptr<VulkanImage>& GetNextImage(VkSemaphore signalSemaphore);
		// Gives the image that was acquired during this frame.
		inline const std::shared_ptr<VulkanImage>& GetCurrentImage() { return m_Images[m_ImageIndex]; }
		inline uint32_t GetCurrentImageIndex() const { return m_ImageIndex; }
		inline uint32_t GetImageCount() const { return m_Images.size(); }
		inline VkFormat GetImageFormat() const { return m_ImageFormat; }
	private:
		VkSwapchainKHR m_SwapChain;
		VkExtent2D m_ImageExtent;
		VkFormat m_ImageFormat;
		std::vector<std::shared_ptr<VulkanImage>> m_Images;
		uint32_t m_ImageIndex = UINT32_MAX;
	};
}
