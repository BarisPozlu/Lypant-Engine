#pragma once

#include <Lypant/Renderer/Sampler.h>
#include <vulkan/vulkan.h>

namespace lypant
{
	class VulkanSampler : public Sampler
	{
	public:
		VulkanSampler(const SamplerSpecification& spec);
		virtual ~VulkanSampler();
		inline VkSampler GetVkSampler() const { return m_Sampler; }
	private:
		VkSampler m_Sampler;
	};
}
