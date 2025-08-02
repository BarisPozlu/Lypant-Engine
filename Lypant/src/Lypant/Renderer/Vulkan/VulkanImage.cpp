#include "lypch.h"
#include "VulkanImage.h"
#include "VulkanGraphicsContext.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include <stb_image.h>

namespace lypant
{
	struct TransitionFlags
	{
		VkPipelineStageFlags SrcStageMask;
		VkAccessFlags SrcAccessMask;
		VkPipelineStageFlags DstStageMask;
		VkAccessFlags DstAccessMask;
	};

	// TODO: duplicate?
	static TransitionFlags GetTransitionFlags(const TransitionSpecification& spec)
	{
		TransitionFlags flags;

		if (spec.OldLayout == VK_IMAGE_LAYOUT_UNDEFINED)
		{
			flags.SrcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			flags.SrcAccessMask = 0;
		}

		else if (spec.OldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		{
			flags.SrcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			flags.SrcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		}

		else if (spec.OldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			flags.SrcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
			flags.SrcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		}

		else
		{
			LY_CORE_ASSERT(false, "transition flags could not be determined");
		}

		if (spec.NewLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		{
			flags.DstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			flags.DstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		}

		else if (spec.NewLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
		{
			flags.DstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			flags.DstAccessMask = 0;
		}

		else if (spec.NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			flags.DstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
			flags.DstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		}

		else
		{
			LY_CORE_ASSERT(false, "transition flags could not be determined");
		}

		return flags;
	}

	static VkFormat GetFormat(const Image2DSpecification& spec, int channels)
	{
		switch (channels)
		{
		case 1:

			if (!spec.FloatingImage)
			{
				return spec.LinearSpace ?  VK_FORMAT_R8_UNORM : VK_FORMAT_R8_SRGB;
			}

			if (!spec.LinearSpace)
			{
				LY_CORE_ASSERT(false, "floating images have to be in linear space");
				return VK_FORMAT_MAX_ENUM;
			}

			return VK_FORMAT_R16_SFLOAT;

		case 2:

			if (!spec.FloatingImage)
			{
				return spec.LinearSpace ? VK_FORMAT_R8G8_UNORM : VK_FORMAT_R8G8_SRGB;
			}

			if (!spec.LinearSpace)
			{
				LY_CORE_ASSERT(false, "floating images have to be in linear space");
				return VK_FORMAT_MAX_ENUM;
			}

			return VK_FORMAT_R16G16_SFLOAT;

		case 3:

			if (!spec.FloatingImage)
			{
				return spec.LinearSpace ? VK_FORMAT_R8G8B8_UNORM : VK_FORMAT_R8G8B8_SRGB;
			}

			if (!spec.LinearSpace)
			{
				LY_CORE_ASSERT(false, "floating images have to be in linear space");
				return VK_FORMAT_MAX_ENUM;
			}

			return VK_FORMAT_R16G16B16_SFLOAT;

		case 4:

			if (!spec.FloatingImage)
			{
				return spec.LinearSpace ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_R8G8B8A8_SRGB;
			}

			if (!spec.LinearSpace)
			{
				LY_CORE_ASSERT(false, "floating images have to be in linear space");
				return VK_FORMAT_MAX_ENUM;
			}

			return VK_FORMAT_R16G16B16A16_SFLOAT;
		}

		LY_CORE_ASSERT(false, "Number of channels is not supported");
		return VK_FORMAT_MAX_ENUM;
	}

	// Returns bytes per pixel
	static uint32_t GetSizeFromFormat(VkFormat format)
	{
		switch (format)
		{
			case VK_FORMAT_R8_UNORM:			return 1;
			case VK_FORMAT_R8_SRGB:				return 1;
			case VK_FORMAT_R16_SFLOAT:			return 2;

			case VK_FORMAT_R8G8_UNORM:			return 2;
			case VK_FORMAT_R8G8_SRGB:			return 2;
			case VK_FORMAT_R16G16_SFLOAT:		return 4;

			case VK_FORMAT_R8G8B8_UNORM:		return 3;
			case VK_FORMAT_R8G8B8_SRGB:			return 3;
			case VK_FORMAT_R16G16B16_SFLOAT:	return 6;

			case VK_FORMAT_R8G8B8A8_UNORM:		return 4;
			case VK_FORMAT_R8G8B8A8_SRGB:		return 4;
			case VK_FORMAT_R16G16B16A16_SFLOAT: return 8;
		}

		LY_CORE_ASSERT(false, "Unknown format");
		return 0;
	}

	VulkanImage2D::VulkanImage2D(const std::string& path, const Image2DSpecification& spec)
	{
		auto& GraphicsContext = VulkanGraphicsContext::Get();

		int width;
		int height;
		int channels;
		void* buffer;

		if (spec.FloatingImage)
		{
			buffer = stbi_loadf(path.c_str(), &width, &height, &channels, 0);
		}

		else
		{
			buffer = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}

		LY_CORE_ASSERT(buffer, "Failed to load the image");

		m_Extent.width = width;
		m_Extent.height = height;

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = m_Extent.height;
		imageInfo.extent.height = m_Extent.width;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1; // TODO:
		imageInfo.arrayLayers = 1;
		imageInfo.format = GetFormat(spec, channels);
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT; // TODO: Not sure if this is needed

		vmaCreateImage(GraphicsContext.GetAllocator(), &imageInfo, &allocInfo, &m_Image, &m_Allocation, nullptr);

		uint32_t size = width * height * GetSizeFromFormat(imageInfo.format);
		VulkanStagingBuffer stagingBuffer(size);

		memcpy(stagingBuffer.GetMappedMemory(), buffer, size);

		{
			VkBufferImageCopy region{};
			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;
			region.imageExtent = { m_Extent.width, m_Extent.height, 1 };

			VulkanImmediateCommandScope scope;

			TransitionImage(scope.GetCommandBuffer(), { VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL });

			vkCmdCopyBufferToImage(scope.GetCommandBuffer(), stagingBuffer.GetBuffer(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

			//TODO: I don't know if I should do this
			TransitionImage(scope.GetCommandBuffer(), { VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
		}

		stbi_image_free(buffer);

		//TODO: Generate Mip map if needed here
	}

	VulkanImage2D::VulkanImage2D(uint32_t width, uint32_t height, uint32_t channels, void* data, const Image2DSpecification& spec)
	{
		auto& GraphicsContext = VulkanGraphicsContext::Get();

		m_Extent.width = width;
		m_Extent.height = height;

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = m_Extent.height;
		imageInfo.extent.height = m_Extent.width;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1; // TODO:
		imageInfo.arrayLayers = 1;
		imageInfo.format = GetFormat(spec, channels);
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT; // TODO: Not sure if this is needed

		vmaCreateImage(GraphicsContext.GetAllocator(), &imageInfo, &allocInfo, &m_Image, &m_Allocation, nullptr);

		uint32_t size = width * height * GetSizeFromFormat(imageInfo.format);
		VulkanStagingBuffer stagingBuffer(size);

		memcpy(stagingBuffer.GetMappedMemory(), data, size);

		{
			VkBufferImageCopy region{};
			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;
			region.imageExtent = { m_Extent.width, m_Extent.height, 1 };

			VulkanImmediateCommandScope scope;

			TransitionImage(scope.GetCommandBuffer(), { VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL });

			vkCmdCopyBufferToImage(scope.GetCommandBuffer(), stagingBuffer.GetBuffer(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

			//TODO: I don't know if I should do this
			TransitionImage(scope.GetCommandBuffer(), { VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL });
		}

		//TODO: Generate Mip map if needed here
	}

	VulkanImage2D::VulkanImage2D(VkImage image, VkImageView imageView, VkExtent2D imageExtent, VkFormat imageFormat)
	{
		m_Image = image;
		m_ImageView = imageView;
		m_Allocation = nullptr;
		m_Extent = imageExtent;
		m_Format = imageFormat;
	}

	VulkanImage2D::~VulkanImage2D()
	{
		const auto& graphicsContext = VulkanGraphicsContext::Get();
		vkDestroyImageView(graphicsContext.GetDevice(), m_ImageView, nullptr);

		if (m_Allocation)
		{
			//TODO
		}
	}

	void VulkanImage2D::TransitionImage(VkCommandBuffer commandBuffer, const TransitionSpecification& spec)
	{
		VkImageMemoryBarrier imageMemoryBarrier{};
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.oldLayout = spec.OldLayout;
		imageMemoryBarrier.newLayout = spec.NewLayout;
		imageMemoryBarrier.image = m_Image;
		imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageMemoryBarrier.subresourceRange.baseMipLevel = spec.BaseMip;
		imageMemoryBarrier.subresourceRange.levelCount = spec.MipCount;
		imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
		imageMemoryBarrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

		TransitionFlags flags = GetTransitionFlags(spec);

		imageMemoryBarrier.srcAccessMask = flags.SrcAccessMask;
		imageMemoryBarrier.dstAccessMask = flags.DstAccessMask;
	
		vkCmdPipelineBarrier(commandBuffer, flags.SrcStageMask, flags.DstStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
	}
}
