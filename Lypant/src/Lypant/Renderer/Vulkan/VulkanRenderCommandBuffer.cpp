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
	}

	VulkanRenderCommandBuffer::~VulkanRenderCommandBuffer()
	{
		DestroyCommandResources();
	}

	void VulkanRenderCommandBuffer::BeginCommands()
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		vkWaitForFences(graphicsContext.GetDevice(), 1, &GetCurrentFrame().FrameFinishedFence, false, UINT64_MAX);

		graphicsContext.GetDeletionQueue().Flush();

		graphicsContext.GetSwapChain().OnFrameBegin(GetCurrentFrame().ImageReceivedSemaphore);

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

		graphicsContext.m_CurrentFrameIndex = (graphicsContext.m_CurrentFrameIndex + 1) % VulkanGraphicsContext::s_MaxFramesInFlight;
	}

	void VulkanRenderCommandBuffer::BeginSubpass(const Subpass& subpass, bool IsImmediate)
	{
		VkCommandBuffer commandBuffer = IsImmediate ? m_ImmediateCommandBuffer.GetVkCommandBuffer() : GetCurrentFrame().CommandBuffer;

		const auto& graphicsContext = VulkanGraphicsContext::Get();

		const VulkanSubpass& vulkanSubpass = reinterpret_cast<const VulkanSubpass&>(subpass);

		VkPipeline pipeline = vulkanSubpass.GetGraphicsPipeline()->GetVkPipeline();
		const auto& shader = vulkanSubpass.GetShader();
		const auto& descriptorSet = vulkanSubpass.GetDescriptorSet();
		const auto& renderTarget = vulkanSubpass.GetRenderTarget();
		const auto& uniformBuffer = vulkanSubpass.GetUniformBuffer();

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

		if (descriptorSet)
		{
			VkDescriptorSet vkDescriptorSet = descriptorSet->GetVkDescriptorSet();
			if (uniformBuffer->IsDynamic())
			{
				uint32_t dynamicOffset = vulkanSubpass.GetUniformBuffer()->GetSize() * graphicsContext.GetCurrentFrameIndex();
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->GetPipelineLayout(), 0, 1, &vkDescriptorSet, 1, &dynamicOffset);
			}
			
			else
			{
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->GetPipelineLayout(), 0, 1, &vkDescriptorSet, 0, nullptr);
			}
		}

		const VkRenderingInfo& renderingInfo = renderTarget->PrepareForRendering(commandBuffer);

		vkCmdBeginRendering(commandBuffer, &renderingInfo);

		VkViewport viewport{};
		viewport.x = renderingInfo.renderArea.offset.x;
		viewport.y = renderingInfo.renderArea.offset.y;
		viewport.width = renderingInfo.renderArea.extent.width;
		viewport.height = renderingInfo.renderArea.extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &renderingInfo.renderArea);
	}

	void VulkanRenderCommandBuffer::EndSubpass(const Subpass& subpass, bool IsImmediate)
	{
		VkCommandBuffer commandBuffer = IsImmediate ? m_ImmediateCommandBuffer.GetVkCommandBuffer() : GetCurrentFrame().CommandBuffer;

		vkCmdEndRendering(commandBuffer);
	}

	void VulkanRenderCommandBuffer::DrawMesh(const Mesh& mesh, const std::shared_ptr<Shader>& shader, uint32_t instanceCount, bool IsImmediate)
	{
		VkCommandBuffer commandBuffer = IsImmediate ? m_ImmediateCommandBuffer.GetVkCommandBuffer() : GetCurrentFrame().CommandBuffer;

		const auto& vkVertexBuffer = reinterpret_cast<const std::shared_ptr<VulkanVertexBuffer>&>(mesh.GetVertexBuffer());
		const auto& vkIndexBuffer = reinterpret_cast<const std::shared_ptr<VulkanIndexBuffer>&>(mesh.GetIndexBuffer());
		const auto& vkShader = reinterpret_cast<const std::shared_ptr<VulkanShader>&>(shader);
		VkDeviceAddress vertexBufferAddress = vkVertexBuffer->GetDeviceAddress();

		vkCmdPushConstants(commandBuffer, vkShader->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VkDeviceAddress), &vertexBufferAddress);
		vkCmdBindIndexBuffer(commandBuffer, vkIndexBuffer->GetVkBuffer(), 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(commandBuffer, vkIndexBuffer->GetIndexCount(), instanceCount, 0, 0, 0);
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

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (int i = 0; i < VulkanGraphicsContext::s_MaxFramesInFlight; i++)
		{
			vkCreateCommandPool(graphicsContext.GetDevice(), &commandPoolInfo, nullptr, &m_FrameData[i].CommandPool);
			commandBufferInfo.commandPool = m_FrameData[i].CommandPool;
			vkAllocateCommandBuffers(graphicsContext.GetDevice(), &commandBufferInfo, &m_FrameData[i].CommandBuffer);

			vkCreateSemaphore(graphicsContext.GetDevice(), &semaphoreInfo, nullptr, &m_FrameData[i].ImageReceivedSemaphore);
			vkCreateFence(graphicsContext.GetDevice(), &fenceInfo, nullptr, &m_FrameData[i].FrameFinishedFence);
		}

		m_RenderFinishedSemaphores.resize(graphicsContext.GetSwapChain().GetImageCount());

		for (auto& semaphore : m_RenderFinishedSemaphores)
		{
			vkCreateSemaphore(graphicsContext.GetDevice(), &semaphoreInfo, nullptr, &semaphore);
		}
	}

	void VulkanRenderCommandBuffer::DestroyCommandResources()
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();
		auto& frameData = m_FrameData;
		auto& renderFinishedSemaphores = m_RenderFinishedSemaphores;

		graphicsContext.GetDeletionQueue().PushFunction([frameData, renderFinishedSemaphores]()
			{
				auto& graphicsContext = VulkanGraphicsContext::Get();

				for (int i = 0; i < VulkanGraphicsContext::s_MaxFramesInFlight; i++)
				{
					vkDestroyCommandPool(graphicsContext.GetDevice(), frameData[i].CommandPool, nullptr);
					vkDestroyFence(graphicsContext.GetDevice(), frameData[i].FrameFinishedFence, nullptr);
					vkDestroySemaphore(graphicsContext.GetDevice(), frameData[i].ImageReceivedSemaphore, nullptr);
				}

				for (auto& semaphore : renderFinishedSemaphores)
				{
					vkDestroySemaphore(graphicsContext.GetDevice(), semaphore, nullptr);
				}
			});
	}
}