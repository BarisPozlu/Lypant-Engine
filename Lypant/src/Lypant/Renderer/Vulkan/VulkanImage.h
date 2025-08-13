#pragma once

#include <Lypant/Renderer/Image.h>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include "VulkanSampler.h"

namespace lypant
{
	// By default all mips and levels get transitioned
	// It is possible to specify which mip levels should be transitioned
	struct TransitionSpecification
	{
		TransitionSpecification(VkImageLayout layout) : NewLayout(layout), BaseMip(0), MipCount(VK_REMAINING_MIP_LEVELS) {}
		TransitionSpecification(VkImageLayout layout, uint32_t baseMip, uint32_t mipCount) : NewLayout(layout), BaseMip(baseMip), MipCount(mipCount) {}

		VkImageLayout NewLayout;
		uint32_t BaseMip;
		uint32_t MipCount;
	};

	class VulkanImage : public Image
	{
	public:
		//TODO: Aspect is always color change that
		VulkanImage(const std::string& path, const ImageSpecification& spec);
		VulkanImage(const void* data, const ImageSpecification& spec);
		VulkanImage(VkImage image, VkImageView imageView, VkExtent2D m_ImageExtent, VkFormat m_ImageFormat);
		virtual ~VulkanImage();

		inline VkImage GetImage() const { return m_Image; }
		inline VkImageView GetImageView() const { return m_ImageView; }
		inline VkExtent2D GetImageExtent() const { return m_Extent; }
		inline VkSampler GetSampler() const { return m_Sampler->GetVkSampler(); }

		//TODO: Aspect is always color change that
		void TransitionImage(VkCommandBuffer commandBuffer, const TransitionSpecification& spec);
	private:
		VkImage m_Image;
		VkImageView m_ImageView = VK_NULL_HANDLE;
		VmaAllocation m_Allocation;
		VkExtent2D m_Extent;
		VkFormat m_Format;
		// NOTE: In the future when multiple inital layouts are supported should change this
		VkImageLayout m_CurrentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		// TODO: Samplers should not be created per image. Write the code so that we get the sampler we want from somewhere else
		std::unique_ptr<VulkanSampler> m_Sampler;
	private:
		friend class VulkanSwapChain;
	};
}
