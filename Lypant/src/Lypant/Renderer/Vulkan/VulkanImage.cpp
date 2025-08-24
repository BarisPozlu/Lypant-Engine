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
	static TransitionFlags GetTransitionFlags(const TransitionSpecification& spec, VkImageLayout oldLayout)
	{
		TransitionFlags flags;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED)
		{
			flags.SrcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			flags.SrcAccessMask = 0;
		}

		else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		{
			flags.SrcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			flags.SrcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		}

		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
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

		else if (spec.NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			flags.DstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			flags.DstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		}

		else
		{
			LY_CORE_ASSERT(false, "transition flags could not be determined");
		}

		return flags;
	}

	static VkFormat GetFormat(const ImageParams& spec, int channels)
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

	// Returns bytes per pixel.
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

	static VkImageUsageFlags GetImageUsageFlags(int imageUsageFlags)
	{
		VkImageUsageFlags flags = 0;

		if (imageUsageFlags & ImageUsageFlagsSample)
		{
			flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
		}

		if (imageUsageFlags & ImageUsageFlagsColorAttachment)
		{
			flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		}

		if (imageUsageFlags & ImageUsageFlagsDepthAttachment)
		{
			flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		}

		if (imageUsageFlags & ImageUsageFlagsTransferSrc)
		{
			flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}

		if (imageUsageFlags & ImageUsageFlagsTransferDst)
		{
			flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}

		return flags;
	}

	VulkanImage::VulkanImage(const std::string& path, const ImageSpecification& spec)
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		m_ImageType = spec.Type;

		int width;
		int height;
		int channels;
		void* buffer;

		if (spec.Params.FloatingImage)
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
		m_Format = GetFormat(spec.Params, channels);

		CreateImage(spec);
		UploadData(buffer);
		stbi_image_free(buffer);

		m_Sampler = std::make_unique<VulkanSampler>(spec.Params.SamplerSpec);

		CreateImageViews(spec);

		//TODO: Generate Mip map if needed here
	}

	VulkanImage::VulkanImage(const void* data, const ImageSpecification& spec)
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		m_ImageType = spec.Type;

		m_Extent.width = spec.Width;
		m_Extent.height = spec.Height;
		m_Format = GetFormat(spec.Params, spec.Channels);

		CreateImage(spec);

		if (data)
		{
			UploadData(data);
		}

		m_Sampler = std::make_unique<VulkanSampler>(spec.Params.SamplerSpec);

		//TODO: Generate Mip map if needed here
	}

	VulkanImage::VulkanImage(VkImage image, VkImageView imageView, VkExtent2D imageExtent, VkFormat imageFormat)
	{
		m_Image = image;
		m_ImageViews.push_back(imageView);
		m_Allocation = nullptr;
		m_Extent = imageExtent;
		m_Format = imageFormat;
		m_ImageType = ImageType::Image2D;
	}

	VulkanImage::~VulkanImage()
	{
		const auto& graphicsContext = VulkanGraphicsContext::Get();

		for (VkImageView view : m_ImageViews)
		{
			vkDestroyImageView(graphicsContext.GetDevice(), view, nullptr);
		}

		if (m_Allocation)
		{
			vmaDestroyImage(graphicsContext.GetAllocator(), m_Image, m_Allocation);
		}
	}

	void VulkanImage::TransitionLayout(VkCommandBuffer commandBuffer, const TransitionSpecification& spec)
	{
		if (m_CurrentLayout == spec.NewLayout) return;

		VkImageMemoryBarrier imageMemoryBarrier{};
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.oldLayout = m_CurrentLayout;
		imageMemoryBarrier.newLayout = spec.NewLayout;
		imageMemoryBarrier.image = m_Image;
		imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageMemoryBarrier.subresourceRange.baseMipLevel = spec.BaseMip;
		imageMemoryBarrier.subresourceRange.levelCount = spec.MipCount;
		imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
		imageMemoryBarrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

		TransitionFlags flags = GetTransitionFlags(spec, m_CurrentLayout);

		imageMemoryBarrier.srcAccessMask = flags.SrcAccessMask;
		imageMemoryBarrier.dstAccessMask = flags.DstAccessMask;
	
		vkCmdPipelineBarrier(commandBuffer, flags.SrcStageMask, flags.DstStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

		m_CurrentLayout = spec.NewLayout;
	}

	void VulkanImage::CreateImage(const ImageSpecification& spec)
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = m_Extent.width;
		imageInfo.extent.height = m_Extent.height;
		imageInfo.extent.depth = spec.Depth;
		imageInfo.mipLevels = 1; // TODO:
		imageInfo.arrayLayers = spec.Layers;
		imageInfo.format = m_Format;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = GetImageUsageFlags(spec.UsageFlags);
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT; // TODO:

		if (m_ImageType == ImageType::Cubemap || m_ImageType == ImageType::CubemapArray)
		{
			imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		}

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT; // TODO: Not sure if this is needed

		vmaCreateImage(graphicsContext.GetAllocator(), &imageInfo, &allocInfo, &m_Image, &m_Allocation, nullptr);
	}

	void VulkanImage::UploadData(const void* buffer)
	{
		uint32_t size = m_Extent.width * m_Extent.height * GetSizeFromFormat(m_Format);
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

			TransitionLayout(scope.GetCommandBuffer(), { VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL });

			vkCmdCopyBufferToImage(scope.GetCommandBuffer(), stagingBuffer.GetBuffer(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

			//TODO: I don't know if I should do this
			TransitionLayout(scope.GetCommandBuffer(), { VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL });
		}
	}

	void VulkanImage::CreateImageViews(const ImageSpecification& spec)
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_Image;
		viewInfo.format = m_Format;
		viewInfo.subresourceRange.aspectMask = spec.UsageFlags & ImageUsageFlagsDepthAttachment ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0; // TODO
		viewInfo.subresourceRange.levelCount = 1; // TODO
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = spec.Layers;

		switch (m_ImageType)
		{
			case ImageType::Image2D:

				m_ImageViews.resize(1);
				viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				vkCreateImageView(graphicsContext.GetDevice(), &viewInfo, nullptr, &m_ImageViews[0]);

				break;
			case ImageType::Image2DArray:

				m_ImageViews.resize(1);
				viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
				vkCreateImageView(graphicsContext.GetDevice(), &viewInfo, nullptr, &m_ImageViews[0]);

				break;
			case ImageType::Cubemap:

				if (spec.UsageFlags & (ImageUsageFlagsColorAttachment | ImageUsageFlagsDepthAttachment))
				{
					m_ImageViews.resize(2);
					viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
					vkCreateImageView(graphicsContext.GetDevice(), &viewInfo, nullptr, &m_ImageViews[1]);
				}

				else
				{
					m_ImageViews.resize(1);
				}
				
				viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
				vkCreateImageView(graphicsContext.GetDevice(), &viewInfo, nullptr, &m_ImageViews[0]);

				break;
			case ImageType::CubemapArray:

				if (spec.UsageFlags & (ImageUsageFlagsColorAttachment | ImageUsageFlagsDepthAttachment))
				{
					m_ImageViews.resize(2);
					viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
					vkCreateImageView(graphicsContext.GetDevice(), &viewInfo, nullptr, &m_ImageViews[1]);
				}

				else
				{
					m_ImageViews.resize(1);
				}

				viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
				vkCreateImageView(graphicsContext.GetDevice(), &viewInfo, nullptr, &m_ImageViews[0]);
				
				break;

			default: LY_CORE_ASSERT(false, "Invalid image type");
		}
	}
}
