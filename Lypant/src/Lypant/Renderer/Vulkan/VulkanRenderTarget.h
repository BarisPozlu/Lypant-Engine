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
		const VkRenderingInfo& PrepareForRendering(VkCommandBuffer commandBuffer);
		inline const std::shared_ptr<VulkanImage>& GetColorBuffer() const { return m_ColorBuffer; }
		inline const std::shared_ptr<VulkanImage>& GetDepthStencilBuffer() const { return m_DepthStencilBuffer; }
	protected:
		std::shared_ptr<VulkanImage> m_ColorBuffer;		
		std::shared_ptr<VulkanImage> m_DepthStencilBuffer;
		VkRenderingAttachmentInfo m_ColorBufferAttachmentInfo{};
		VkRenderingAttachmentInfo m_DepthStencilAttachmentInfo{};
		VkRenderingInfo m_RenderingInfo{};
	};
}