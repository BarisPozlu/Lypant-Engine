#include <lypch.h>
#include "VulkanDescriptorSet.h"
#include "VulkanGraphicsContext.h"

namespace lypant
{
	std::vector<VkDescriptorPool> VulkanDescriptorSetAllocator::s_DescriptorPools;
	int VulkanDescriptorSetAllocator::s_PoolIndex = -1;

	VkDescriptorSet VulkanDescriptorSetAllocator::Allocate(VkDescriptorSetLayout layout)
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		VkDescriptorSetAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocateInfo.descriptorPool = s_DescriptorPools[s_PoolIndex];
		allocateInfo.descriptorSetCount = 1;
		allocateInfo.pSetLayouts = &layout;

		VkDescriptorSet descriptorSet;

		VkResult result = vkAllocateDescriptorSets(graphicsContext.GetDevice(), &allocateInfo, &descriptorSet);

		if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL)
		{
			CreatePool();
			allocateInfo.descriptorPool = s_DescriptorPools[s_PoolIndex];
			vkAllocateDescriptorSets(graphicsContext.GetDevice(), &allocateInfo, &descriptorSet);
		}

		return descriptorSet;
	}

	void VulkanDescriptorSetAllocator::Init()
	{
		CreatePool();
	}

	void VulkanDescriptorSetAllocator::Shutdown()
	{
		for (VkDescriptorPool pool : s_DescriptorPools)
		{
			vkDestroyDescriptorPool(VulkanGraphicsContext::Get().GetDevice(), pool, nullptr);
		}
	}

	void VulkanDescriptorSetAllocator::CreatePool()
	{
		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSize.descriptorCount = 1024 * 4;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.maxSets = 1024;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;

		VkDescriptorPool pool;

		vkCreateDescriptorPool(VulkanGraphicsContext::Get().GetDevice(), &poolInfo, nullptr, &pool);

		s_DescriptorPools.push_back(pool);
		s_PoolIndex++;
	}
}
