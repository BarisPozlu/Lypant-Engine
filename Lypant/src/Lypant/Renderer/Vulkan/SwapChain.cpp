#include "lypch.h"
#include "Lypant/Renderer/SwapChain.h"
#include "Lypant/Renderer/GraphicsContext.h"
#include <glm/glm.hpp>
#include "Lypant/Core/Application.h"

namespace lypant
{
	SwapChain::SwapChain()
	{
		const auto& graphicsContext = Application::Get().GetWindow().GetGraphicsContext();
		const auto& deviceSurfaceDetails = graphicsContext->GetDeviceSurfaceDetails();

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

		VkPresentModeKHR selectedMode = VK_PRESENT_MODE_FIFO_KHR;

		for (int i = 0; i < deviceSurfaceDetails.Modes.size(); i++)
		{
			if (deviceSurfaceDetails.Modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				selectedMode = VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}
		}

		VkExtent2D imageExtent;

		if (deviceSurfaceDetails.Capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			imageExtent = deviceSurfaceDetails.Capabilities.currentExtent;
		}

		else
		{
			imageExtent.width = Application::Get().GetWindow().GetFramebufferWidth(), Application::Get().GetWindow().GetFramebufferHeight();
			imageExtent.height = Application::Get().GetWindow().GetFramebufferWidth(), Application::Get().GetWindow().GetFramebufferHeight();
		}

		uint32_t imageCount = deviceSurfaceDetails.Capabilities.minImageCount + 1;

		LY_ASSERT(!(deviceSurfaceDetails.Capabilities.maxImageCount > 0 && imageCount > deviceSurfaceDetails.Capabilities.maxImageCount), "Swap chain image count is not supported");

		VkSwapchainCreateInfoKHR swapChainInfo{};
		swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapChainInfo.surface = graphicsContext->GetSurface();
		swapChainInfo.minImageCount = imageCount;
		swapChainInfo.imageFormat = selectedFormat.format;
		swapChainInfo.imageColorSpace = selectedFormat.colorSpace;
		swapChainInfo.imageExtent = imageExtent;
		swapChainInfo.imageArrayLayers = 1;
		swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapChainInfo.preTransform = deviceSurfaceDetails.Capabilities.currentTransform;
		swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapChainInfo.presentMode = selectedMode;
		swapChainInfo.clipped = VK_TRUE;
		swapChainInfo.oldSwapchain = VK_NULL_HANDLE;

		vkCreateSwapchainKHR(graphicsContext->GetDevice(), &swapChainInfo, nullptr, &m_SwapChain);
	}

	SwapChain::~SwapChain()
	{
		vkDestroySwapchainKHR(Application::Get().GetWindow().GetGraphicsContext()->GetDevice(), m_SwapChain, nullptr);
	}
}
