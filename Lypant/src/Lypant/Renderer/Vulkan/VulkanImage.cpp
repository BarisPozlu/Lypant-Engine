#include "lypch.h"
#include "VulkanImage.h"
#include "VulkanGraphicsContext.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/integer.hpp>

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

		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
		{
			flags.SrcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
			flags.SrcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		}

		else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			flags.SrcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			flags.SrcAccessMask = VK_ACCESS_SHADER_READ_BIT;
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

		else if (spec.NewLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
		{
			flags.DstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
			flags.DstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		}

		else if (spec.NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			flags.DstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			flags.DstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		}

		else if (spec.NewLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
		{
			flags.DstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			flags.DstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}

		else
		{
			LY_CORE_ASSERT(false, "transition flags could not be determined");
		}

		return flags;
	}

	static VkFormat GetFormatFromSpec(const ImageSpecification& spec, int channels)
	{
		const ImageParams& params = spec.Params;

		if (spec.UsageFlags & ImageUsageFlagsDepthAttachment)
		{
			return params.FloatingImage ? VK_FORMAT_D32_SFLOAT : VK_FORMAT_D16_UNORM;
		}

		switch (channels)
		{
		case 1:

			if (!params.FloatingImage)
			{
				return params.LinearSpace ?  VK_FORMAT_R8_UNORM : VK_FORMAT_R8_SRGB;
			}

			if (!params.LinearSpace)
			{
				LY_CORE_ASSERT(false, "floating images have to be in linear space");
				return VK_FORMAT_MAX_ENUM;
			}

			return VK_FORMAT_R16_SFLOAT;

		case 2:

			if (!params.FloatingImage)
			{
				return params.LinearSpace ? VK_FORMAT_R8G8_UNORM : VK_FORMAT_R8G8_SRGB;
			}

			if (!params.LinearSpace)
			{
				LY_CORE_ASSERT(false, "floating images have to be in linear space");
				return VK_FORMAT_MAX_ENUM;
			}

			return VK_FORMAT_R16G16_SFLOAT;

		case 3:

			if (!params.FloatingImage)
			{
				return params.LinearSpace ? VK_FORMAT_R8G8B8_UNORM : VK_FORMAT_R8G8B8_SRGB;
			}

			if (!params.LinearSpace)
			{
				LY_CORE_ASSERT(false, "floating images have to be in linear space");
				return VK_FORMAT_MAX_ENUM;
			}

			return VK_FORMAT_R16G16B16_SFLOAT;

		case 4:

			if (!params.FloatingImage)
			{
				return params.LinearSpace ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_R8G8B8A8_SRGB;
			}

			if (!params.LinearSpace)
			{
				LY_CORE_ASSERT(false, "floating images have to be in linear space");
				return VK_FORMAT_MAX_ENUM;
			}

			return VK_FORMAT_R32G32B32A32_SFLOAT;
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
			case VK_FORMAT_R32G32B32A32_SFLOAT: return 16;
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
		m_LayerCount = spec.Layers;
		m_Path = path;

		int width;
		int height;
		int channels;
		void* buffer;
		// TODO: CHANGED FOR TESTING, REMOVE
		if (spec.Params.FloatingImage)
		{
			//buffer = stbi_loadf(path.c_str(), &width, &height, &channels, 0);
			buffer = stbi_loadf(path.c_str(), &width, &height, &channels, 4);
			channels = 4;
		}

		else
		{
			buffer = stbi_load(path.c_str(), &width, &height, &channels, 4);
			channels = 4;
		}

		LY_CORE_ASSERT(buffer, "Failed to load the image");

		m_Extent.width = width;
		m_Extent.height = height;
		m_Format = GetFormatFromSpec(spec, channels);

		CreateImage(spec);
		UploadData(buffer);
		stbi_image_free(buffer);

		m_Sampler = std::make_unique<VulkanSampler>(spec.Params.SamplerSpec);

		CreateImageViews(spec);

		// NOTE: Engine will only generate mip maps for 2D images, for other types it will simply allocate memory.
		if (spec.Params.GenerateMipMap && m_ImageType == ImageType::Image2D)
		{
			GenerateMipMaps();
		}
	}

	VulkanImage::VulkanImage(const void* data, const ImageSpecification& spec)
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		m_ImageType = spec.Type;
		m_LayerCount = spec.Layers;

		m_Extent.width = spec.Width;
		m_Extent.height = spec.Height;
		m_Format = GetFormatFromSpec(spec, spec.Channels);

		CreateImage(spec);

		if (data)
		{
			UploadData(data);
		}

		m_Sampler = std::make_unique<VulkanSampler>(spec.Params.SamplerSpec);

		CreateImageViews(spec);

		// NOTE: Engine will only generate mip maps for 2D images, for other types it will simply allocate memory.
		if (spec.Params.GenerateMipMap && m_ImageType == ImageType::Image2D)
		{
			GenerateMipMaps();
		}
	}

	VulkanImage::VulkanImage(VkImage image, VkImageView imageView, VkExtent2D imageExtent, VkFormat imageFormat)
	{
		m_Image = image;
		m_ImageViews.push_back(imageView);
		m_Allocation = nullptr;
		m_Extent = imageExtent;
		m_Format = imageFormat;
		m_ImageType = ImageType::Image2D;
		m_LayerCount = 1;
		m_MipCount = 1;
	}

	VulkanImage::~VulkanImage()
	{
		if (m_Path.size())
		{
			s_Cache.erase(m_Path);
		}

		auto& graphicsContext = VulkanGraphicsContext::Get();

		auto& views = m_ImageViews;
		VmaAllocation allocation = m_Allocation;
		VkImage image = m_Image;

		graphicsContext.GetDeletionQueue().PushFunction([views, allocation, image]()
			{
				const auto& graphicsContext = VulkanGraphicsContext::Get();

				for (VkImageView view : views)
				{
					vkDestroyImageView(graphicsContext.GetDevice(), view, nullptr);
				}

				if (allocation)
				{
					vmaDestroyImage(graphicsContext.GetAllocator(), image, allocation);
				}
			});
	}

	void VulkanImage::TransitionLayout(VkCommandBuffer commandBuffer, const TransitionSpecification& spec)
	{
		if (m_CurrentLayout == spec.NewLayout) return;

		VkImageMemoryBarrier imageMemoryBarrier{};
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.oldLayout = m_CurrentLayout;
		imageMemoryBarrier.newLayout = spec.NewLayout;
		imageMemoryBarrier.image = m_Image;
		// TODO: Aspect mask is not set properly
		imageMemoryBarrier.subresourceRange.aspectMask = m_Format == VK_FORMAT_D16_UNORM ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		imageMemoryBarrier.subresourceRange.baseMipLevel = spec.BaseMip;
		imageMemoryBarrier.subresourceRange.levelCount = spec.MipCount;
		imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
		imageMemoryBarrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

		TransitionFlags flags = GetTransitionFlags(spec, m_CurrentLayout);

		imageMemoryBarrier.srcAccessMask = flags.SrcAccessMask;
		imageMemoryBarrier.dstAccessMask = flags.DstAccessMask;
	
		vkCmdPipelineBarrier(commandBuffer, flags.SrcStageMask, flags.DstStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

		if (spec.BaseMip == 0 && spec.MipCount == VK_REMAINING_MIP_LEVELS) m_CurrentLayout = spec.NewLayout;
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
		if (spec.Params.GenerateMipMap)
		{
			m_MipCount = glm::floor(glm::log2(glm::max(m_Extent.width, m_Extent.height))) + 1;
		}
		else
		{
			m_MipCount = 1;
		}
		imageInfo.mipLevels = m_MipCount;
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
		auto& stagingBuffer = reinterpret_cast<std::shared_ptr<VulkanBuffer>&>(Buffer::CreateStagingBuffer(size));

		memcpy(stagingBuffer->GetMappedMemory(), buffer, size);
		
		VulkanImmediateCommandBuffer commandBuffer;

		VkBufferImageCopy region{};
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageExtent = { m_Extent.width, m_Extent.height, 1 };

		commandBuffer.BeginCommands();

		TransitionLayout(commandBuffer.GetVkCommandBuffer(), { VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL });

		vkCmdCopyBufferToImage(commandBuffer.GetVkCommandBuffer(), stagingBuffer->GetVkBuffer(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		commandBuffer.EndCommands();
	}

	void VulkanImage::CreateImageViews(const ImageSpecification& spec)
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_Image;
		viewInfo.format = m_Format;
		viewInfo.subresourceRange.aspectMask = spec.UsageFlags & ImageUsageFlagsDepthAttachment ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = m_MipCount;
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
					spec.Params.CreateViewsPerMipMap ?  m_ImageViews.resize(2 + m_MipCount - 1) : m_ImageViews.resize(2);
					viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
					vkCreateImageView(graphicsContext.GetDevice(), &viewInfo, nullptr, &m_ImageViews[1]);

					viewInfo.subresourceRange.levelCount = 1;

					for (int i = 2; i < m_ImageViews.size(); i++)
					{
						viewInfo.subresourceRange.baseMipLevel = i - 1;
						vkCreateImageView(graphicsContext.GetDevice(), &viewInfo, nullptr, &m_ImageViews[i]);
					}
				}

				else
				{
					LY_CORE_ASSERT(spec.Params.CreateViewsPerMipMap, "Views per mip map is only supported for images that are used as attachments");
					m_ImageViews.resize(1);
				}
				
				viewInfo.subresourceRange.baseMipLevel = 0;
				viewInfo.subresourceRange.levelCount = m_MipCount;
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

	void VulkanImage::GenerateMipMaps()
	{
		VulkanImmediateCommandBuffer cmd;

		cmd.BeginCommands();

		VkImageSubresourceLayers subresource{};
		subresource.baseArrayLayer = 0;
		subresource.layerCount = 1;
		subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

		int mipWidth = m_Extent.width;
		int mipHeight = m_Extent.height;

		for (uint32_t i = 0; i < m_MipCount - 1; i++)
		{
			TransitionLayout(cmd.GetVkCommandBuffer(), { VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, i, 1 });

			VkImageBlit blit{};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };

			mipWidth = mipWidth > 1 ? mipWidth / 2 : 1;
			mipHeight = mipHeight > 1 ? mipHeight / 2 : 1;

			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { mipWidth, mipHeight, 1 };

			subresource.mipLevel = i;
			blit.srcSubresource = subresource;
			
			subresource.mipLevel = i + 1;
			blit.dstSubresource = subresource;

			vkCmdBlitImage(cmd.GetVkCommandBuffer(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);
		}

		TransitionLayout(cmd.GetVkCommandBuffer(), { VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_MipCount - 1, 1 });
		m_CurrentLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

		cmd.EndCommands();
	}

	VkImageView VulkanImage::GetImageView(ImageViewType type, int mipLevel) const
	{
		LY_CORE_ASSERT(!(type == ImageViewType::Sample && mipLevel != 0), "mipLevel should only be specifed for attachment views");

		if (type == ImageViewType::Attachment)
		{
			if (m_ImageType == ImageType::Cubemap || m_ImageType == ImageType::CubemapArray)
			{
				return m_ImageViews[static_cast<int>(ImageViewType::Attachment) + mipLevel];
			}
		}

		return m_ImageViews[static_cast<int>(ImageViewType::Sample)];
	}
}
