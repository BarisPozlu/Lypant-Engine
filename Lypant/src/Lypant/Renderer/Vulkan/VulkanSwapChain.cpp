#include "lypch.h"
#include "VulkanSwapChain.h"
#include "VulkanGraphicsContext.h"
#include "Lypant/Core/Application.h"
#include "VulkanRenderTarget.h"

namespace lypant
{
	VulkanSwapChain::VulkanSwapChain(VkDevice device, VkSurfaceKHR surface, const VulkanDeviceSurfaceDetails& deviceSurfaceDetails)
	{
		VkSurfaceFormatKHR selectedFormat = deviceSurfaceDetails.Formats[0];

		for (int i = 0; i < deviceSurfaceDetails.Formats.size(); i++)
		{
			const auto& format = deviceSurfaceDetails.Formats[i];

			if (format.format == VK_FORMAT_B8G8R8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				selectedFormat = format;
				break;
			}
		}

		m_ImageFormat = selectedFormat.format;

		VkPresentModeKHR selectedMode = VK_PRESENT_MODE_FIFO_KHR;

		for (int i = 0; i < deviceSurfaceDetails.Modes.size(); i++)
		{
			if (deviceSurfaceDetails.Modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				selectedMode = VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}
		}

		if (deviceSurfaceDetails.Capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			m_ImageExtent = deviceSurfaceDetails.Capabilities.currentExtent;
		}

		else
		{
			m_ImageExtent.width = Application::Get().GetWindow().GetFramebufferWidth(), Application::GetWindow().GetFramebufferHeight();
			m_ImageExtent.height = Application::Get().GetWindow().GetFramebufferWidth(), Application::GetWindow().GetFramebufferHeight();
		}

		uint32_t imageCount = deviceSurfaceDetails.Capabilities.minImageCount + 1;

		LY_CORE_ASSERT(!(deviceSurfaceDetails.Capabilities.maxImageCount > 0 && imageCount > deviceSurfaceDetails.Capabilities.maxImageCount), "Swap chain image count is not supported");

		VkSwapchainCreateInfoKHR swapChainInfo{};
		swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapChainInfo.surface = surface;
		swapChainInfo.minImageCount = imageCount;
		swapChainInfo.imageFormat = selectedFormat.format;
		swapChainInfo.imageColorSpace = selectedFormat.colorSpace;
		swapChainInfo.imageExtent = m_ImageExtent;
		swapChainInfo.imageArrayLayers = 1;
		swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapChainInfo.preTransform = deviceSurfaceDetails.Capabilities.currentTransform;
		swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapChainInfo.presentMode = selectedMode;
		swapChainInfo.clipped = VK_TRUE;
		swapChainInfo.oldSwapchain = VK_NULL_HANDLE;

		vkCreateSwapchainKHR(device, &swapChainInfo, nullptr, &m_SwapChain);

		vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, nullptr);
		std::vector<VkImage> images(imageCount);
		vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, images.data());

		m_Images.resize(imageCount);

		for (int i = 0; i < imageCount; i++)
		{
			VkImageView imageView;

			VkImageViewCreateInfo imageViewInfo{};
			imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewInfo.image = images[i];
			imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewInfo.format = selectedFormat.format;
			imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewInfo.subresourceRange.baseMipLevel = 0;
			imageViewInfo.subresourceRange.levelCount = 1;
			imageViewInfo.subresourceRange.baseArrayLayer = 0;
			imageViewInfo.subresourceRange.layerCount = 1;

			vkCreateImageView(device, &imageViewInfo, nullptr, &imageView);

			m_Images[i] = std::make_shared<VulkanImage>(images[i], imageView, m_ImageExtent, selectedFormat.format);
		}

		m_RenderTarget = std::make_shared<VulkanRenderTarget>();

		// NOTE: Attached image is not valid since it is not acquired from the swap chain yet.
		// This does not cause problems, since before rendering to this render target, OnFrameBegin gets called and its color buffer is updated.
		// The reason why this is done is so that passes that might use this render target can get its color buffer's format for graphics pipeline creation.
		m_RenderTarget->AttachColorBuffer(m_Images[m_ImageIndex]);
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		const auto& graphicsContext = VulkanGraphicsContext::Get();
		vkDestroySwapchainKHR(graphicsContext.GetDevice(), m_SwapChain, nullptr);
	}

	void VulkanSwapChain::OnFrameBegin(VkSemaphore imageReceivedSemaphore)
	{
		const auto& graphicsContext = VulkanGraphicsContext::Get();

		vkAcquireNextImageKHR(graphicsContext.GetDevice(), m_SwapChain, UINT64_MAX, imageReceivedSemaphore, VK_NULL_HANDLE, &m_ImageIndex);

		const auto& image = m_Images[m_ImageIndex];

		image->m_CurrentLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		m_RenderTarget->AttachColorBuffer(image);
	}

	void VulkanSwapChain::OnFrameEnd(VkCommandBuffer commandBuffer)
	{
		m_Images[m_ImageIndex]->TransitionLayout(commandBuffer, { VK_IMAGE_LAYOUT_PRESENT_SRC_KHR });
	}
}
