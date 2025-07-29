#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace lypant
{
	// By default all the mips and levels get transitioned
	// It is possible to specify which mip levels should be transitioned
	struct TransitionSpecification
	{
		VkImageLayout OldLayout;
		VkImageLayout NewLayout;
		uint32_t BaseMip = 0;
		uint32_t MipCount = VK_REMAINING_MIP_LEVELS;
	};

	// for now it is only created using an already existing image, we will have the option to create from scracth later
	class VulkanImage2D
	{
	public:
		VulkanImage2D(VkImage image, VkImageView imageView, VkExtent2D m_ImageExtent, VkFormat m_ImageFormat);
		virtual ~VulkanImage2D();
		inline VkImage GetImage() const { return m_Image; }
		inline VkImageView GetImageView() const { return m_ImageView; }
		inline VkExtent2D GetImageExtent() const { return m_ImageExtent; }

		//TODO: Aspect is always color change that
		void TransitionImage(VkCommandBuffer commandBuffer, const TransitionSpecification& spec);
	private:
		VkImage m_Image;
		VkImageView m_ImageView;
		VmaAllocation m_Allocation;
		VkExtent2D m_ImageExtent;
		VkFormat m_ImageFormat;
	};
}
