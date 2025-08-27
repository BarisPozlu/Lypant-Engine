#include <lypch.h>
#include "VulkanRenderCommandBuffer.h"
#include "VulkanGraphicsContext.h"
#include "VulkanSwapChain.h"
#include "VulkanImage.h"
#include "VulkanRenderPass.h"
#include "VulkanBuffer.h"

namespace lypant
{
	VulkanRenderCommandBuffer::VulkanRenderCommandBuffer()
	{
		CreateCommandResources();
		CreateSyncResources();
	}

	VulkanRenderCommandBuffer::~VulkanRenderCommandBuffer()
	{
		vkQueueWaitIdle(VulkanGraphicsContext::Get().GetGraphicsQueue());
		DestroyCommandResources();
		DestroySyncResources();
	}

	void VulkanRenderCommandBuffer::BeginCommands()
	{
		vkWaitForFences(VulkanGraphicsContext::Get().GetDevice(), 1, &GetCurrentFrame().FrameFinishedFence, false, UINT64_MAX);

		VulkanGraphicsContext::Get().GetSwapChain().OnFrameBegin(GetCurrentFrame().ImageReceivedSemaphore);

		vkResetCommandBuffer(GetCurrentFrame().CommandBuffer, 0);

		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		vkBeginCommandBuffer(GetCurrentFrame().CommandBuffer, &commandBufferBeginInfo);
	}

	void VulkanRenderCommandBuffer::EndCommands()
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();
		auto& swapChain = graphicsContext.GetSwapChain();

		swapChain.OnFrameEnd(GetCurrentFrame().CommandBuffer);

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

		m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % s_MaxFramesInFlight;
	}

	void VulkanRenderCommandBuffer::BeginSubpass(const Subpass& subpass)
	{
		const VulkanSubpass& vulkanSubpass = reinterpret_cast<const VulkanSubpass&>(subpass);

		VkPipeline pipeline = vulkanSubpass.GetGraphicsPipeline()->GetVkPipeline();
		const auto& shader = vulkanSubpass.GetShader();
		const auto& descriptorSet = vulkanSubpass.GetDescriptorSet();
		const auto& renderTarget = vulkanSubpass.GetRenderTarget();
		const auto& uniformBuffer = vulkanSubpass.GetUniformBuffer();

		vkCmdBindPipeline(GetCurrentFrame().CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

		if (descriptorSet)
		{
			VkDescriptorSet vkDescriptorSet = descriptorSet->GetVkDescriptorSet();
			if (uniformBuffer->IsDynamic())
			{
				uint32_t dynamicOffset = vulkanSubpass.GetUniformBuffer()->GetSize() * m_CurrentFrameIndex;
				vkCmdBindDescriptorSets(GetCurrentFrame().CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->GetPipelineLayout(), 0, 1, &vkDescriptorSet, 1, &dynamicOffset);
			}
			
			else
			{
				vkCmdBindDescriptorSets(GetCurrentFrame().CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->GetPipelineLayout(), 0, 1, &vkDescriptorSet, 0, nullptr);
			}
		}

		const VkRenderingInfo& renderingInfo = renderTarget->PrepareForRendering(GetCurrentFrame().CommandBuffer);

		vkCmdBeginRendering(GetCurrentFrame().CommandBuffer, &renderingInfo);

		VkViewport viewport{};
		viewport.x = renderingInfo.renderArea.offset.x;
		viewport.y = renderingInfo.renderArea.offset.y;
		viewport.width = renderingInfo.renderArea.extent.width;
		viewport.height = renderingInfo.renderArea.extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vkCmdSetViewport(GetCurrentFrame().CommandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(GetCurrentFrame().CommandBuffer, 0, 1, &renderingInfo.renderArea);
	}

	void VulkanRenderCommandBuffer::EndSubpass(const Subpass& subpass)
	{
		vkCmdEndRendering(GetCurrentFrame().CommandBuffer);
	}

	void VulkanRenderCommandBuffer::DrawMesh(const Mesh& mesh, const std::shared_ptr<Shader>& shader, uint32_t instanceCount)
	{
		const auto& vkVertexBuffer = reinterpret_cast<const std::shared_ptr<VulkanVertexBuffer>&>(mesh.GetVertexBuffer());
		const auto& vkIndexBuffer = reinterpret_cast<const std::shared_ptr<VulkanIndexBuffer>&>(mesh.GetIndexBuffer());
		const auto& vkShader = reinterpret_cast<const std::shared_ptr<VulkanShader>&>(shader);
		VkDeviceAddress vertexBufferAddress = vkVertexBuffer->GetDeviceAddress();

		vkCmdPushConstants(GetCurrentFrame().CommandBuffer, vkShader->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VkDeviceAddress), &vertexBufferAddress);
		vkCmdBindIndexBuffer(GetCurrentFrame().CommandBuffer, vkIndexBuffer->GetVkBuffer(), 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(GetCurrentFrame().CommandBuffer, vkIndexBuffer->GetIndexCount(), instanceCount, 0, 0, 0);
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