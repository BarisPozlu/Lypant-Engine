#include "lypch.h"
#include "VulkanImage.h"
#include <Lypant/Core/Application.h>
#include "VulkanGraphicsContext.h"

namespace lypant
{
	struct TransitionFlags
	{
		VkPipelineStageFlags SrcStageMask;
		VkAccessFlags SrcAccessMask;
		VkPipelineStageFlags DstStageMask;
		VkAccessFlags DstAccessMask;
	};

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

	VulkanImage2D::VulkanImage2D(VkImage image, VkImageView imageView, VkExtent2D imageExtent, VkFormat imageFormat)
	{
		m_Image = image;
		m_ImageView = imageView;
		m_Allocation = nullptr;
		m_ImageExtent = imageExtent;
		m_ImageFormat = imageFormat;
	}

	VulkanImage2D::~VulkanImage2D()
	{
		const auto& graphicsContext = VulkanGraphicsContext::Get();
		vkDestroyImageView(graphicsContext->GetDevice(), m_ImageView, nullptr);

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
