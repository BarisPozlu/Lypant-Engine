#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace lypant
{
	class VulkanDescriptorSetAllocator
	{
	public:
		static VkDescriptorSet Allocate(VkDescriptorSetLayout layout);
	private:
		static void Init();
		static void Shutdown();
		static void CreatePool();
	private:
		static std::vector<VkDescriptorPool> s_DescriptorPools;
		static int s_PoolIndex;
	private:
		friend class VulkanGraphicsContext;
	};
}