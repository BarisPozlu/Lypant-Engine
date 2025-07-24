#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Image.h"

namespace lypant
{
	class SwapChain
	{
	public:
		SwapChain();
		~SwapChain();
	private:
		VkSwapchainKHR m_SwapChain;
		std::vector<Image> m_Images;
	};
}