#pragma once

#include <Lypant/Renderer/Sampler.h>
#include <vulkan/vulkan.h>

namespace lypant
{
	class VulkanSampler
	{
	public:
		VulkanSampler(SamplerSpecification spec);
		~VulkanSampler();
		inline VkSampler GetVkSampler() const { return m_Sampler; }
	private:
		VkSampler m_Sampler;
	};
}
