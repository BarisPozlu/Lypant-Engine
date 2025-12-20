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

	enum class ImageViewType
	{
		Sample, Attachment
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
		VkImageView GetImageView(ImageViewType type = ImageViewType::Sample, int mipLevel = 0) const;
		inline VkExtent2D GetImageExtent() const { return m_Extent; }
		inline VkSampler GetSampler() const { return m_Sampler->GetVkSampler(); }
		inline VkFormat GetFormat() const { return m_Format; }
		inline uint32_t GetLayerCount() const { return m_LayerCount; }

		//TODO: Aspect is always color change that
		void TransitionLayout(VkCommandBuffer commandBuffer, const TransitionSpecification& spec);
	private:
		void CreateImage(const ImageSpecification& spec);
		void UploadData(const void* buffer);
		void CreateImageViews(const ImageSpecification& spec);
		void GenerateMipMaps();
	private:
		VkImage m_Image;
		std::vector<VkImageView> m_ImageViews;
		VmaAllocation m_Allocation;
		VkExtent2D m_Extent;
		VkFormat m_Format;
		ImageType m_ImageType;
		VkImageLayout m_CurrentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		uint32_t m_LayerCount;
		uint32_t m_MipCount;
		// TODO: Samplers should not be created per image. Write the code so that we get the sampler we want from somewhere else
		std::unique_ptr<VulkanSampler> m_Sampler;
		std::string m_Path;
	private:
		friend class VulkanSwapChain;
	};
}
