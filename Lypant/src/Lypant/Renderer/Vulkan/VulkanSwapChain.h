#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "VulkanImage.h"

namespace lypant
{
	struct VulkanDeviceSurfaceDetails;
	class VulkanRenderTarget;

	// TODO: Give option to use vsync, adaptive vsync or none of them
	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(VkDevice device, VkSurfaceKHR surface, const VulkanDeviceSurfaceDetails& deviceSurfaceDetails);
		~VulkanSwapChain();
		// If the swap chain is outdated and could not begin frame return false.
		bool OnFrameBegin(VkSemaphore imageReceivedSemaphore);
		void OnFrameEnd(VkCommandBuffer commandBuffer);
		inline VkSwapchainKHR GetSwapChain() const { return m_SwapChain; }
		inline uint32_t GetCurrentImageIndex() const { return m_ImageIndex; }
		inline uint32_t GetImageCount() const { return m_Images.size(); }
		inline VkFormat GetImageFormat() const { return m_ImageFormat; }
		inline const std::shared_ptr<VulkanRenderTarget>& GetDefaultRenderTarget() const { return m_RenderTarget; }
	private:
		VkSwapchainKHR m_SwapChain;
		VkExtent2D m_ImageExtent;
		VkFormat m_ImageFormat;
		std::vector<std::shared_ptr<VulkanImage>> m_Images;
		std::shared_ptr<VulkanRenderTarget> m_RenderTarget;
		uint32_t m_ImageIndex = 0;
	};
}
