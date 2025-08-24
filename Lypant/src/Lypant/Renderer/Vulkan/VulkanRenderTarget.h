#pragma once

#include <lypant/Renderer/RenderTarget.h>
#include <vulkan/vulkan.h>
#include "VulkanImage.h"

namespace lypant
{
	class VulkanRenderTarget : public RenderTarget
	{
	public:
		VulkanRenderTarget() = default;
		virtual ~VulkanRenderTarget() = default;
		virtual void AttachColorBuffer(const std::shared_ptr<Image>& image) override;
		virtual void AttachDepthStencilBuffer(const std::shared_ptr<Image>& image) override;
		// PrepareForRendering maybe should be done within the render command buffer instead of here
		virtual const VkRenderingInfo& PrepareForRendering(class VulkanRenderCommandBuffer& renderCommandBuffer);
	protected:
		std::shared_ptr<VulkanImage> m_ColorBuffer;		
		std::shared_ptr<VulkanImage> m_DepthStencilBuffer;
		VkRenderingAttachmentInfo m_ColorBufferAttachmentInfo{};
		VkRenderingAttachmentInfo m_DepthStencilAttachmentInfo{};
		VkRenderingInfo m_RenderingInfo{};
	};

	class VulkanDefaultRenderTarget : public VulkanRenderTarget
	{
	public:
		VulkanDefaultRenderTarget() = default;
		virtual ~VulkanDefaultRenderTarget() = default;
		virtual const VkRenderingInfo& PrepareForRendering(class VulkanRenderCommandBuffer& renderCommandBuffer) override;
	};
}