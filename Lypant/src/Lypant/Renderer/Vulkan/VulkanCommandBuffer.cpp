#include <lypch.h>
#include "VulkanCommandBuffer.h"
#include "VulkanGraphicsContext.h"

namespace lypant
{
	VulkanImmediateCommandBuffer::VulkanImmediateCommandBuffer()
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		VkCommandBufferAllocateInfo commandBufferInfo{};
		commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferInfo.commandPool = s_CommandPool;
		commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferInfo.commandBufferCount = 1;
		
		vkAllocateCommandBuffers(graphicsContext.GetDevice(), &commandBufferInfo, &m_CommandBuffer);
		
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		
		vkCreateFence(graphicsContext.GetDevice(), &fenceInfo, nullptr, &m_Fence);
	}

	VulkanImmediateCommandBuffer::~VulkanImmediateCommandBuffer()
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		vkDestroyFence(graphicsContext.GetDevice(), m_Fence, nullptr);
		vkFreeCommandBuffers(graphicsContext.GetDevice(), s_CommandPool, 1, &m_CommandBuffer);
	}

	void VulkanImmediateCommandBuffer::BeginCommands()
	{
		vkResetCommandBuffer(m_CommandBuffer, 0);
		vkResetFences(VulkanGraphicsContext::Get().GetDevice(), 1, &m_Fence);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		vkBeginCommandBuffer(m_CommandBuffer, &beginInfo);
	}

	void VulkanImmediateCommandBuffer::EndCommands()
	{
		vkEndCommandBuffer(m_CommandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffer;

		vkQueueSubmit(VulkanGraphicsContext::Get().GetGraphicsQueue(), 1, &submitInfo, m_Fence);

		vkWaitForFences(VulkanGraphicsContext::Get().GetDevice(), 1, &m_Fence, false, UINT64_MAX);
	}

	void VulkanImmediateCommandBuffer::Init(VkDevice device, uint32_t graphicsQueueFamilyIndex)
	{
		VkCommandPoolCreateInfo commandPoolInfo{};
		commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolInfo.queueFamilyIndex = graphicsQueueFamilyIndex;

		vkCreateCommandPool(device, &commandPoolInfo, nullptr, &s_CommandPool);
	}

	void VulkanImmediateCommandBuffer::Shutdown()
	{
		vkDestroyCommandPool(VulkanGraphicsContext::Get().GetDevice(), s_CommandPool, nullptr);
	}
}
