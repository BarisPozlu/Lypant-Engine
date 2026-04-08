#include <lypch.h>
#include "VulkanDescriptorSet.h"
#include "VulkanGraphicsContext.h"
#include "VulkanImage.h"
#include "VulkanBuffer.h"

namespace lypant
{
	std::vector<VkDescriptorPool> VulkanDescriptorSetAllocator::s_DescriptorPools;
	int VulkanDescriptorSetAllocator::s_PoolIndex = -1;

	static VkDescriptorType GetDescriptorTypeFromBuffer(const std::shared_ptr<VulkanBuffer>& buffer)
	{
		switch (buffer->GetBufferType())
		{
			case BufferType::UniformBuffer: if (buffer->IsDynamic()) return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC; return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; break;
			case BufferType::StorageBuffer: if (buffer->IsDynamic()) return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC; return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; break;
		}

		LY_CORE_ASSERT(false, "Unknown buffer type or a buffer type that should not be in a descriptor.");
		return VK_DESCRIPTOR_TYPE_MAX_ENUM;
	}

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
		std::array<VkDescriptorPoolSize, 5> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[0].descriptorCount = 1024 * 4;
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[1].descriptorCount = 1024 * 4;
		poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		poolSizes[2].descriptorCount = 1024 * 4;
		poolSizes[3].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		poolSizes[3].descriptorCount = 1024 * 4;
		poolSizes[4].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
		poolSizes[4].descriptorCount = 1024 * 4;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.maxSets = 1024;
		poolInfo.poolSizeCount = poolSizes.size();
		poolInfo.pPoolSizes = poolSizes.data();

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
		// TODO: For now the descriptor sets are only freed when their pool is destroyed
	}

	void VulkanDescriptorSet::Update(const std::vector<ImageBinding>& imageBindings, const std::vector<BufferBinding>& bufferBindings) const
	{
		int bindingCount = imageBindings.size() + bufferBindings.size();
		std::vector<VkWriteDescriptorSet> setWrites(bindingCount);

		std::vector<VkDescriptorImageInfo> imageInfos(imageBindings.size());

		for (int i = 0; i < imageBindings.size(); i++)
		{
			const ImageBinding& imageBinding = imageBindings[i];
			VkWriteDescriptorSet& setWrite = setWrites[i];
			VkDescriptorImageInfo& imageInfo = imageInfos[i];
			
			const auto& vulkanImage = reinterpret_cast<const std::shared_ptr<VulkanImage>&>(imageBinding.Image);

			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = vulkanImage->GetImageView();
			imageInfo.sampler = vulkanImage->GetSampler();

			setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			setWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			setWrite.descriptorCount = 1;
			setWrite.dstSet = m_DescriptorSet;
			setWrite.dstBinding = imageBinding.Binding;
			setWrite.dstArrayElement = 0;
			setWrite.pImageInfo = &imageInfo;
		}

		std::vector<VkDescriptorBufferInfo> bufferInfos(bufferBindings.size());

		for (int i = 0; i < bufferBindings.size(); i++)
		{
			const BufferBinding& bufferBinding = bufferBindings[i];
			VkWriteDescriptorSet& setWrite = setWrites[imageBindings.size() + i];
			VkDescriptorBufferInfo& bufferInfo = bufferInfos[i];

			const auto& vkBuffer = reinterpret_cast<const std::shared_ptr<VulkanBuffer>&>(bufferBinding.Buffer);

			bufferInfo.buffer = vkBuffer->GetVkBuffer();
			bufferInfo.offset = bufferBinding.Offset;
			bufferInfo.range = bufferBinding.Range;

			setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			setWrite.descriptorType = GetDescriptorTypeFromBuffer(vkBuffer);
			setWrite.descriptorCount = 1;
			setWrite.dstSet = m_DescriptorSet;
			setWrite.dstBinding = bufferBinding.Binding;
			setWrite.dstArrayElement = 0;
			setWrite.pBufferInfo = &bufferInfo;
		}
	
		vkUpdateDescriptorSets(VulkanGraphicsContext::Get().GetDevice(), setWrites.size(), setWrites.data(), 0, nullptr);
	}

	void VulkanDescriptorSet::UpdateWithDescriptorArrays(const std::vector<ImageBinding>& imageBindings, const std::vector<BufferBinding>& bufferBindings) const
	{
		int bindingCount = 1 + bufferBindings.size();
		std::vector<VkWriteDescriptorSet> setWrites(bindingCount);

		std::vector<VkDescriptorImageInfo> imageInfos(imageBindings.size());

		for (int i = 0; i < imageBindings.size(); i++)
		{
			const ImageBinding& imageBinding = imageBindings[i];
			VkDescriptorImageInfo& imageInfo = imageInfos[i];

			const auto& vulkanImage = reinterpret_cast<const std::shared_ptr<VulkanImage>&>(imageBinding.Image);

			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = vulkanImage->GetImageView();
			imageInfo.sampler = vulkanImage->GetSampler();
		}

		setWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		setWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		setWrites[0].descriptorCount = imageBindings.size();
		setWrites[0].dstSet = m_DescriptorSet;
		setWrites[0].dstBinding = imageBindings[0].Binding;
		setWrites[0].dstArrayElement = 0;
		setWrites[0].pImageInfo = imageInfos.data();

		std::vector<VkDescriptorBufferInfo> bufferInfos(bufferBindings.size());

		for (int i = 0; i < bufferBindings.size(); i++)
		{
			const BufferBinding& bufferBinding = bufferBindings[i];
			VkWriteDescriptorSet& setWrite = setWrites[i + 1];
			VkDescriptorBufferInfo& bufferInfo = bufferInfos[i];

			const auto& vkBuffer = reinterpret_cast<const std::shared_ptr<VulkanBuffer>&>(bufferBinding.Buffer);

			bufferInfo.buffer = vkBuffer->GetVkBuffer();
			bufferInfo.offset = bufferBinding.Offset;
			bufferInfo.range = bufferBinding.Range;

			setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			setWrite.descriptorType = GetDescriptorTypeFromBuffer(vkBuffer);
			setWrite.descriptorCount = 1;
			setWrite.dstSet = m_DescriptorSet;
			setWrite.dstBinding = bufferBinding.Binding;
			setWrite.dstArrayElement = 0;
			setWrite.pBufferInfo = &bufferInfo;
		}

		vkUpdateDescriptorSets(VulkanGraphicsContext::Get().GetDevice(), setWrites.size(), setWrites.data(), 0, nullptr);
	}
}
