#include <lypch.h>
#include "VulkanRenderCommandBuffer.h"
#include "VulkanGraphicsContext.h"
#include "VulkanSwapChain.h"
#include "VulkanImage.h"

namespace lypant
{
	VulkanRenderCommandBuffer::VulkanRenderCommandBuffer()
	{
		CreateCommandResources();
		CreateSyncResources();
	}

	VulkanRenderCommandBuffer::~VulkanRenderCommandBuffer()
	{
		vkDeviceWaitIdle(VulkanGraphicsContext::Get().GetDevice());
		DestroyCommandResources();
		DestroySyncResources();
	}

	void VulkanRenderCommandBuffer::BeginCommands()
	{
		vkWaitForFences(VulkanGraphicsContext::Get().GetDevice(), 1, &GetCurrentFrame().FrameFinishedFence, false, UINT64_MAX);

		vkResetCommandBuffer(GetCurrentFrame().CommandBuffer, 0);

		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		vkBeginCommandBuffer(GetCurrentFrame().CommandBuffer, &commandBufferBeginInfo);
	}

	void VulkanRenderCommandBuffer::EndCommands()
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();
		auto& swapChain = graphicsContext.GetSwapChain();

		vkCmdEndRendering(GetCurrentFrame().CommandBuffer);

		swapChain.GetCurrentImage().TransitionImage(GetCurrentFrame().CommandBuffer, { VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR });

		vkEndCommandBuffer(GetCurrentFrame().CommandBuffer);

		vkResetFences(graphicsContext.GetDevice(), 1, &GetCurrentFrame().FrameFinishedFence);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &GetCurrentFrame().ImageReceivedSemaphore;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &GetCurrentFrame().CommandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_RenderFinishedSemaphores[swapChain.GetCurrentImageIndex()];

		vkQueueSubmit(graphicsContext.GetGraphicsQueue(), 1, &submitInfo, GetCurrentFrame().FrameFinishedFence);

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_RenderFinishedSemaphores[swapChain.GetCurrentImageIndex()];
		presentInfo.swapchainCount = 1;
		VkSwapchainKHR vkSwapChain = swapChain.GetSwapChain();
		presentInfo.pSwapchains = &vkSwapChain;
		uint32_t imageIndex = swapChain.GetCurrentImageIndex();
		presentInfo.pImageIndices = &imageIndex;

		vkQueuePresentKHR(graphicsContext.GetGraphicsQueue(), &presentInfo);

		m_CurrentFrame = (m_CurrentFrame + 1) % s_MaxFramesInFlight;
	}

	void VulkanRenderCommandBuffer::SetRenderTargetToDefault()
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();
		auto& image = graphicsContext.GetSwapChain().GetNextImage(GetCurrentFrame().ImageReceivedSemaphore);

		image.TransitionImage(GetCurrentFrame().CommandBuffer, { VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });

		VkRenderingAttachmentInfoKHR renderingAttachmentInfo{};
		renderingAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		renderingAttachmentInfo.imageView = image.GetImageView();
		renderingAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		renderingAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		renderingAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		renderingAttachmentInfo.clearValue = { 0.5f, 0.0f, 0.0f, 1.0f };

		VkRenderingInfoKHR renderingInfo{};
		renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
		renderingInfo.renderArea.offset = { 0, 0 };
		renderingInfo.renderArea.extent = image.GetImageExtent();
		renderingInfo.layerCount = 1;
		renderingInfo.colorAttachmentCount = 1;
		renderingInfo.pColorAttachments = &renderingAttachmentInfo;

		vkCmdBeginRendering(GetCurrentFrame().CommandBuffer, &renderingInfo);
	}

	void VulkanRenderCommandBuffer::CreateCommandResources()
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		VkCommandPoolCreateInfo commandPoolInfo{};
		commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolInfo.queueFamilyIndex = graphicsContext.GetGraphicsQueueFamilyIndex();

		VkCommandBufferAllocateInfo commandBufferInfo{};
		commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferInfo.commandBufferCount = 1;

		for (int i = 0; i < s_MaxFramesInFlight; i++)
		{
			vkCreateCommandPool(graphicsContext.GetDevice(), &commandPoolInfo, nullptr, &m_FrameData[i].CommandPool);
			commandBufferInfo.commandPool = m_FrameData[i].CommandPool;
			vkAllocateCommandBuffers(graphicsContext.GetDevice(), &commandBufferInfo, &m_FrameData[i].CommandBuffer);
		}
	}

	void VulkanRenderCommandBuffer::CreateSyncResources()
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		m_RenderFinishedSemaphores.resize(graphicsContext.GetSwapChain().GetImageCount());

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (auto& semaphore : m_RenderFinishedSemaphores)
		{
			vkCreateSemaphore(graphicsContext.GetDevice(), &semaphoreInfo, nullptr, &semaphore);
		}

		for (int i = 0; i < s_MaxFramesInFlight; i++)
		{
			vkCreateSemaphore(graphicsContext.GetDevice(), &semaphoreInfo, nullptr, &m_FrameData[i].ImageReceivedSemaphore);
			vkCreateFence(graphicsContext.GetDevice(), &fenceInfo, nullptr, &m_FrameData[i].FrameFinishedFence);
		}
	}

	void VulkanRenderCommandBuffer::DestroyCommandResources()
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		for (int i = 0; i < s_MaxFramesInFlight; i++)
		{
			vkDestroyCommandPool(graphicsContext.GetDevice(), m_FrameData[i].CommandPool, nullptr);
		}
	}

	void VulkanRenderCommandBuffer::DestroySyncResources()
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		for (auto& semaphore : m_RenderFinishedSemaphores)
		{
			vkDestroySemaphore(graphicsContext.GetDevice(), semaphore, nullptr);
		}

		for (int i = 0; i < s_MaxFramesInFlight; i++)
		{			
			vkDestroyFence(graphicsContext.GetDevice(), m_FrameData[i].FrameFinishedFence, nullptr);
			vkDestroySemaphore(graphicsContext.GetDevice(), m_FrameData[i].ImageReceivedSemaphore, nullptr);
		}
	}

}