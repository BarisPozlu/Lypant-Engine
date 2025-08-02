#pragma once

#include <Lypant/Renderer/Image.h>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace lypant
{
	// By default all mips and levels get transitioned
	// It is possible to specify which mip levels should be transitioned
	struct TransitionSpecification
	{
		VkImageLayout OldLayout;
		VkImageLayout NewLayout;
		uint32_t BaseMip = 0;
		uint32_t MipCount = VK_REMAINING_MIP_LEVELS;
	};

	class VulkanImage2D : public Image2D
	{
	public:
		//TODO: Aspect is always color change that
		VulkanImage2D(const std::string& path, const Image2DSpecification& spec);
		VulkanImage2D(uint32_t width, uint32_t height, uint32_t channels, void* data, const Image2DSpecification& spec);

		VulkanImage2D(VkImage image, VkImageView imageView, VkExtent2D m_ImageExtent, VkFormat m_ImageFormat);
		virtual ~VulkanImage2D();
		inline VkImage GetImage() const { return m_Image; }
		inline VkImageView GetImageView() const { return m_ImageView; }
		inline VkExtent2D GetImageExtent() const { return m_Extent; }

		//TODO: Aspect is always color change that
		void TransitionImage(VkCommandBuffer commandBuffer, const TransitionSpecification& spec);
	private:
		VkImage m_Image;
		VkImageView m_ImageView;
		VmaAllocation m_Allocation;
		VkExtent2D m_Extent;
		VkFormat m_Format;
	};
}
