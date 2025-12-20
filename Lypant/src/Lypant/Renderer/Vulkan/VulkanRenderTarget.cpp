#include <lypch.h>
#include "VulkanRenderTarget.h"
#include <glm/glm.hpp>

namespace lypant
{
	static VkAttachmentLoadOp GetVkAttachmentLoadOp(AttachmentLoadOperation op)
	{
		switch (op)
		{
			case AttachmentLoadOperation::Load: return VK_ATTACHMENT_LOAD_OP_LOAD;
			case AttachmentLoadOperation::Clear: return VK_ATTACHMENT_LOAD_OP_CLEAR;
			case AttachmentLoadOperation::DontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		}

		LY_CORE_ASSERT(false, "Load operation is not known");
		return VK_ATTACHMENT_LOAD_OP_MAX_ENUM;
	}

	static VkAttachmentStoreOp GetVkAttachmentStoreOp(AttachmentStoreOperation op)
	{
		switch (op)
		{
			case AttachmentStoreOperation::Store: return VK_ATTACHMENT_STORE_OP_STORE;
			case AttachmentStoreOperation::DontCare: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
		}

		LY_CORE_ASSERT(false, "Store operation is not known");
		return VK_ATTACHMENT_STORE_OP_MAX_ENUM;
	}

	void VulkanRenderTarget::AttachColorBuffer(const std::shared_ptr<Image>& image, int mipLevel)
	{
		m_ColorBuffer = reinterpret_cast<const std::shared_ptr<VulkanImage>&>(image);

		m_ColorBufferAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		m_ColorBufferAttachmentInfo.imageView = m_ColorBuffer->GetImageView(ImageViewType::Attachment, mipLevel);
		m_ColorBufferAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		m_ColorBufferAttachmentInfo.clearValue.color = { 0.5f, 0.0f, 0.0f, 1.0f };
		
		m_RenderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
		m_RenderingInfo.renderArea.offset = { 0, 0 };
		m_RenderingInfo.renderArea.extent.width = m_ColorBuffer->GetImageExtent().width * glm::pow(0.5, mipLevel);
		m_RenderingInfo.renderArea.extent.height = m_ColorBuffer->GetImageExtent().height * glm::pow(0.5, mipLevel);
		m_RenderingInfo.layerCount = m_ColorBuffer->GetLayerCount();
		m_RenderingInfo.colorAttachmentCount = 1;
		m_RenderingInfo.pColorAttachments = &m_ColorBufferAttachmentInfo;
	}

	void VulkanRenderTarget::AttachDepthStencilBuffer(const std::shared_ptr<Image>& image)
	{
		m_DepthStencilBuffer = reinterpret_cast<const std::shared_ptr<VulkanImage>&>(image);

		m_DepthStencilAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		m_DepthStencilAttachmentInfo.imageView = m_DepthStencilBuffer->GetImageView(ImageViewType::Attachment);
		m_DepthStencilAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
		m_DepthStencilAttachmentInfo.clearValue.depthStencil.depth = 1.0f;

		m_RenderingInfo.pDepthAttachment = &m_DepthStencilAttachmentInfo;
	}

	const VkRenderingInfo& VulkanRenderTarget::PrepareForRendering(VkCommandBuffer commandBuffer, const RenderTargetOperation& op)
	{
		if (m_ColorBuffer)
		{
			m_ColorBuffer->TransitionLayout(commandBuffer, { VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
			m_ColorBufferAttachmentInfo.loadOp = GetVkAttachmentLoadOp(op.ColorBufferLoadOp);
			m_ColorBufferAttachmentInfo.storeOp = GetVkAttachmentStoreOp(op.ColorBufferStoreOp);
		}

		if (m_DepthStencilBuffer)
		{
			m_DepthStencilBuffer->TransitionLayout(commandBuffer, { VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL });
			m_DepthStencilAttachmentInfo.loadOp = GetVkAttachmentLoadOp(op.DepthBufferLoadOp);
			m_DepthStencilAttachmentInfo.storeOp = GetVkAttachmentStoreOp(op.DepthBufferStoreOp);
		}

		return m_RenderingInfo;
	}
}