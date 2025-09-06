#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <Lypant/Renderer/RenderPass.h>

namespace lypant
{
	class VulkanDescriptorSetAllocator
	{
	public:
		static VkDescriptorSet Allocate(VkDescriptorSetLayout layout);
	private:
		static void Init(VkDevice device);
		static void Shutdown();
		static void CreatePool(VkDevice device);
	private:
		static std::vector<VkDescriptorPool> s_DescriptorPools;
		static int s_PoolIndex;
	private:
		friend class VulkanGraphicsContext;
	};

	class VulkanDescriptorSet
	{
	public:
		VulkanDescriptorSet(VkDescriptorSetLayout layout);
		~VulkanDescriptorSet();
		VkDescriptorSet GetVkDescriptorSet() const { return m_DescriptorSet; }
		void Update(const std::vector<DataBinding>& dataBindings, const std::shared_ptr<class Buffer>& buffer = nullptr) const;
	private:
		VkDescriptorSet m_DescriptorSet;
	};
}