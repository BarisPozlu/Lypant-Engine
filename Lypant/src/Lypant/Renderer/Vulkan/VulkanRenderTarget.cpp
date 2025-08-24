#include <lypch.h>
#include "VulkanRenderTarget.h"
#include "VulkanGraphicsContext.h"
#include "VulkanSwapChain.h"
#include "VulkanRenderCommandBuffer.h"

namespace lypant
{
	void VulkanRenderTarget::AttachColorBuffer(const std::shared_ptr<Image>& image)
	{
		m_ColorBuffer = reinterpret_cast<const std::shared_ptr<VulkanImage>&>(image);

		m_ColorBufferAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		m_ColorBufferAttachmentInfo.imageView = m_ColorBuffer->GetImageView();
		m_ColorBufferAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		m_ColorBufferAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		m_ColorBufferAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		m_ColorBufferAttachmentInfo.clearValue = { 0.5f, 0.0f, 0.0f, 1.0f };
		
		m_RenderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
		m_RenderingInfo.renderArea.offset = { 0, 0 };
		m_RenderingInfo.renderArea.extent = m_ColorBuffer->GetImageExtent();
		m_RenderingInfo.layerCount = 1;
		m_RenderingInfo.colorAttachmentCount = 1;
		m_RenderingInfo.pColorAttachments = &m_ColorBufferAttachmentInfo;
	}

	void VulkanRenderTarget::AttachDepthStencilBuffer(const std::shared_ptr<Image>& image)
	{
		// TODO:
	}

	const VkRenderingInfo& VulkanRenderTarget::PrepareForRendering(VulkanRenderCommandBuffer& renderCommandBuffer)
	{
		m_ColorBuffer->TransitionLayout(renderCommandBuffer.GetCommandBuffer(), { VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });

		return m_RenderingInfo;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const VkRenderingInfo& VulkanDefaultRenderTarget::PrepareForRendering(VulkanRenderCommandBuffer& renderCommandBuffer)
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();
		auto& image = graphicsContext.GetSwapChain().GetNextImage(renderCommandBuffer.GetCurrentFrame().ImageReceivedSemaphore);

		AttachColorBuffer(image);

		m_ColorBuffer->TransitionLayout(renderCommandBuffer.GetCurrentFrame().CommandBuffer, { VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });

		return m_RenderingInfo;
	}
}