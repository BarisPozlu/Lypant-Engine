#include <lypch.h>
#include "VulkanRenderer.h"
#include <Lypant/Core/Application.h>
#include "VulkanGraphicsContext.h"
#include "VulkanSwapChain.h"
#include "VulkanImage.h"

namespace lypant
{
	VulkanRenderer::VulkanRenderer()
	{
		CreateCommandResources();
		CreateSyncResources();
	}

	VulkanRenderer::~VulkanRenderer()
	{
		DestroyCommandResources();
		DestroySyncResources();
	}

	void VulkanRenderer::BeginRendering_Impl()
	{
		vkResetCommandBuffer(GetCurrentFrame().CommandBuffer, 0);

		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		vkBeginCommandBuffer(GetCurrentFrame().CommandBuffer, &commandBufferBeginInfo);
	}

	void VulkanRenderer::EndRendering_Impl()
	{
		const auto& graphicsContext = VulkanGraphicsContext::Get();
		auto& swapChain = graphicsContext->GetSwapChain();

		vkEndCommandBuffer(GetCurrentFrame().CommandBuffer);

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

		vkQueueSubmit(graphicsContext->GetGraphicsQueue(), 1, &submitInfo, GetCurrentFrame().FrameFinishedFence);

		m_CurrentFrame = (m_CurrentFrame + 1) % s_MaxFramesInFlight;
	}

	void VulkanRenderer::ClearImage_Impl()
	{
		VkClearColorValue clearValue{ { 1.0f, 0.0f, 0.0f, 1.0f } };

		VkImageSubresourceRange subImage{};
		subImage.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subImage.baseMipLevel = 0;
		subImage.levelCount = VK_REMAINING_MIP_LEVELS;
		subImage.baseArrayLayer = 0;
		subImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

		const auto& graphicsContext = VulkanGraphicsContext::Get();
		auto& swapChainImage = graphicsContext->GetSwapChain().GetNextImage(GetCurrentFrame().ImageReceivedSemaphore);

		swapChainImage.TransitionImage(GetCurrentFrame().CommandBuffer, { VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL });

		vkCmdClearColorImage(GetCurrentFrame().CommandBuffer, swapChainImage.GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearValue, 1, &subImage);

		swapChainImage.TransitionImage(GetCurrentFrame().CommandBuffer, { VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR });
	}

	void VulkanRenderer::CreateCommandResources()
	{
		const auto& graphicsContext = VulkanGraphicsContext::Get();

		VkCommandPoolCreateInfo commandPoolInfo{};
		commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolInfo.queueFamilyIndex = graphicsContext->GetGraphicsQueueFamilyIndex();

		VkCommandBufferAllocateInfo commandBufferInfo{};
		commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferInfo.commandBufferCount = 1;

		for (int i = 0; i < s_MaxFramesInFlight; i++)
		{
			vkCreateCommandPool(graphicsContext->GetDevice(), &commandPoolInfo, nullptr, &m_FrameData[i].CommandPool);
			commandBufferInfo.commandPool = m_FrameData[i].CommandPool;
			vkAllocateCommandBuffers(graphicsContext->GetDevice(), &commandBufferInfo, &m_FrameData[0].CommandBuffer);
		}
	}

	void VulkanRenderer::CreateSyncResources()
	{
		const auto& graphicsContext = VulkanGraphicsContext::Get();

		m_RenderFinishedSemaphores.resize(graphicsContext->GetSwapChain().GetImageCount());

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (auto& semaphore : m_RenderFinishedSemaphores)
		{
			vkCreateSemaphore(graphicsContext->GetDevice(), &semaphoreInfo, nullptr, &semaphore);
		}

		for (int i = 0; i < s_MaxFramesInFlight; i++)
		{
			vkCreateSemaphore(graphicsContext->GetDevice(), &semaphoreInfo, nullptr, &m_FrameData[i].ImageReceivedSemaphore);
			vkCreateFence(graphicsContext->GetDevice(), &fenceInfo, nullptr, &m_FrameData[i].FrameFinishedFence);
		}
	}

	void VulkanRenderer::DestroyCommandResources()
	{
		const auto& graphicsContext = VulkanGraphicsContext::Get();

		for (int i = 0; i < s_MaxFramesInFlight; i++)
		{
			vkDestroyCommandPool(graphicsContext->GetDevice(), m_FrameData[i].CommandPool, nullptr);
		}
	}

	void VulkanRenderer::DestroySyncResources()
	{
		const auto& graphicsContext = VulkanGraphicsContext::Get();

		for (auto& semaphore : m_RenderFinishedSemaphores)
		{
			vkDestroySemaphore(graphicsContext->GetDevice(), semaphore, nullptr);
		}

		for (int i = 0; i < s_MaxFramesInFlight; i++)
		{			
			vkDestroyFence(graphicsContext->GetDevice(), m_FrameData[i].FrameFinishedFence, nullptr);
			vkDestroySemaphore(graphicsContext->GetDevice(), m_FrameData[i].ImageReceivedSemaphore, nullptr);
		}
	}


}