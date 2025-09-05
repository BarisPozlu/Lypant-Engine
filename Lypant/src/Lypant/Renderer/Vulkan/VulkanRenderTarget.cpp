#include <lypch.h>
#include "VulkanRenderTarget.h"

namespace lypant
{
	void VulkanRenderTarget::AttachColorBuffer(const std::shared_ptr<Image>& image)
	{
		m_ColorBuffer = reinterpret_cast<const std::shared_ptr<VulkanImage>&>(image);

		m_ColorBufferAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		m_ColorBufferAttachmentInfo.imageView = m_ColorBuffer->GetImageView(ImageViewType::Attachment);
		m_ColorBufferAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		m_ColorBufferAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		m_ColorBufferAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		m_ColorBufferAttachmentInfo.clearValue = { 0.5f, 0.0f, 0.0f, 1.0f };
		
		m_RenderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
		m_RenderingInfo.renderArea.offset = { 0, 0 };
		m_RenderingInfo.renderArea.extent = m_ColorBuffer->GetImageExtent();
		m_RenderingInfo.layerCount = m_ColorBuffer->GetLayerCount();
		m_RenderingInfo.colorAttachmentCount = 1;
		m_RenderingInfo.pColorAttachments = &m_ColorBufferAttachmentInfo;
	}

	void VulkanRenderTarget::AttachDepthStencilBuffer(const std::shared_ptr<Image>& image)
	{
		// TODO:
	}

	const VkRenderingInfo& VulkanRenderTarget::PrepareForRendering(VkCommandBuffer commandBuffer, bool shouldClear)
	{
		m_ColorBuffer->TransitionLayout(commandBuffer, { VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });

		if (shouldClear)
		{
			m_ColorBufferAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		}

		else
		{
			m_ColorBufferAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		}

		return m_RenderingInfo;
	}
}