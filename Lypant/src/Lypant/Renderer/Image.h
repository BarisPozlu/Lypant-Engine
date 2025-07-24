#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace lypant
{

	// this is a placeholder for now for the swap chain, it has no implementation
	class Image
	{
	public:

	private:
		VkImage m_Image;
		VkImageView m_ImageView;
	};
}
