#include <lypch.h>
#include "VulkanCommandBuffer.h"
#include "VulkanGraphicsContext.h"

namespace lypant
{
	VulkanImmediateCommandScope::VulkanImmediateCommandScope()
	{
		vkResetCommandBuffer(s_CommandBuffer, 0);
		vkResetFences(VulkanGraphicsContext::Get().GetDevice(), 1, &s_Fence);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		vkBeginCommandBuffer(s_CommandBuffer, &beginInfo);
	}

	VulkanImmediateCommandScope::~VulkanImmediateCommandScope()
	{
		vkEndCommandBuffer(s_CommandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &s_CommandBuffer;

		vkQueueSubmit(VulkanGraphicsContext::Get().GetGraphicsQueue(), 1, &submitInfo, s_Fence);

		vkWaitForFences(VulkanGraphicsContext::Get().GetDevice(), 1, &s_Fence, false, UINT64_MAX);
	}

	void VulkanImmediateCommandScope::Init()
	{
		VkCommandPoolCreateInfo commandPoolInfo{};
		commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolInfo.queueFamilyIndex = VulkanGraphicsContext::Get().GetGraphicsQueueFamilyIndex();

		vkCreateCommandPool(VulkanGraphicsContext::Get().GetDevice(), &commandPoolInfo, nullptr, &s_CommandPool);

		VkCommandBufferAllocateInfo commandBufferInfo{};
		commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferInfo.commandPool = s_CommandPool;
		commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferInfo.commandBufferCount = 1;

		vkAllocateCommandBuffers(VulkanGraphicsContext::Get().GetDevice(), &commandBufferInfo, &s_CommandBuffer);

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		vkCreateFence(VulkanGraphicsContext::Get().GetDevice(), &fenceInfo, nullptr, &s_Fence);
	}

	void VulkanImmediateCommandScope::Shutdown()
	{
		vkDestroyFence(VulkanGraphicsContext::Get().GetDevice(), s_Fence, nullptr);
		vkDestroyCommandPool(VulkanGraphicsContext::Get().GetDevice(), s_CommandPool, nullptr);
	}
}
