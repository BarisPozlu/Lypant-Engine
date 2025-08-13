#include <lypch.h>
#include "VulkanDescriptorSet.h"
#include "VulkanGraphicsContext.h"
#include "VulkanImage.h"

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
			CreatePool(VulkanGraphicsContext::Get().GetDevice());
			allocateInfo.descriptorPool = s_DescriptorPools[s_PoolIndex];
			vkAllocateDescriptorSets(graphicsContext.GetDevice(), &allocateInfo, &descriptorSet);
		}

		return descriptorSet;
	}

	void VulkanDescriptorSetAllocator::Init(VkDevice device)
	{
		CreatePool(device);
	}

	void VulkanDescriptorSetAllocator::Shutdown()
	{
		for (VkDescriptorPool pool : s_DescriptorPools)
		{
			vkDestroyDescriptorPool(VulkanGraphicsContext::Get().GetDevice(), pool, nullptr);
		}
	}

	void VulkanDescriptorSetAllocator::CreatePool(VkDevice device)
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

		vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool);

		s_DescriptorPools.push_back(pool);
		s_PoolIndex++;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VulkanDescriptorSet::VulkanDescriptorSet(VkDescriptorSetLayout layout)
	{
		m_DescriptorSet = VulkanDescriptorSetAllocator::Allocate(layout);
	}

	VulkanDescriptorSet::~VulkanDescriptorSet()
	{

	}

	void VulkanDescriptorSet::Update(const std::vector<DataBinding>& dataBindings)
	{
		std::vector<VkWriteDescriptorSet> setWrites(dataBindings.size());

		std::vector<VkDescriptorImageInfo> imageInfos;

		for (int i = 0; i < dataBindings.size(); i++)
		{
			const DataBinding& dataBinding = dataBindings[i];
			VkWriteDescriptorSet& setWrite = setWrites[i];

			if (dataBinding.Image)
			{
				const auto& vulkanImage = reinterpret_cast<const std::shared_ptr<VulkanImage>&>(dataBinding.Image);

				VkDescriptorImageInfo imageInfo{};
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = vulkanImage->GetImageView();
				imageInfo.sampler = vulkanImage->GetSampler();

				imageInfos.push_back(imageInfo);

				setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				setWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				setWrite.descriptorCount = 1;
				setWrite.dstSet = m_DescriptorSet;
				setWrite.dstBinding = dataBinding.Binding;
				setWrite.dstArrayElement = 0;
				setWrite.pImageInfo = &imageInfos.back();
			}

		}

		vkUpdateDescriptorSets(VulkanGraphicsContext::Get().GetDevice(), setWrites.size(), setWrites.data(), 0, nullptr);
	}
}
