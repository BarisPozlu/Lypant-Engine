#include <lypch.h>
#include "VulkanSampler.h"
#include "VulkanGraphicsContext.h"

namespace lypant
{
	static VkSamplerAddressMode GetVulkanWrappingOption(ImageWrappingOption wrappingOption)
	{
		switch (wrappingOption)
		{
			case lypant::ImageWrappingOption::Repeat:		return VK_SAMPLER_ADDRESS_MODE_REPEAT;
			case lypant::ImageWrappingOption::ClampEdge:	return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			case lypant::ImageWrappingOption::ClampBorder:	return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		}

		LY_CORE_ASSERT(false, "Unknown wrapping option");
		return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
	}

	static VkFilter GetVulkanFilteringOption(ImageFilteringOption filteringOption)
	{
		switch (filteringOption)
		{
			case lypant::ImageFilteringOption::Nearest:		return VK_FILTER_NEAREST;
			case lypant::ImageFilteringOption::Linear:		return VK_FILTER_LINEAR;
		}

		LY_CORE_ASSERT(false, "Unknown filtering option");
		return VK_FILTER_MAX_ENUM;
	}

	VulkanSampler::VulkanSampler(const SamplerSpecification& spec)
	{
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = GetVulkanFilteringOption(spec.FilteringOption);
		samplerInfo.minFilter = GetVulkanFilteringOption(spec.FilteringOption);
		samplerInfo.addressModeU = GetVulkanWrappingOption(spec.WrappingOption);
		samplerInfo.addressModeV = GetVulkanWrappingOption(spec.WrappingOption);
		samplerInfo.addressModeW = GetVulkanWrappingOption(spec.WrappingOption);
		samplerInfo.maxAnisotropy = VulkanGraphicsContext::Get().GetMaxSamplerAnisotropy();
		samplerInfo.anisotropyEnable = true;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.maxLod = VK_LOD_CLAMP_NONE;

		vkCreateSampler(VulkanGraphicsContext::Get().GetDevice(), &samplerInfo, nullptr, &m_Sampler);
	}

	VulkanSampler::~VulkanSampler()
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		VkSampler sampler = m_Sampler;

		graphicsContext.GetDeletionQueue().PushFunction([sampler]()
			{
				vkDestroySampler(VulkanGraphicsContext::Get().GetDevice(), sampler, nullptr);
			});
		
	}
}