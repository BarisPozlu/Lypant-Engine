#include "lypch.h"
#include "Sampler.h"
#include "GraphicsContext.h"
#include "Vulkan/VulkanSampler.h"

namespace lypant
{
	std::shared_ptr<Sampler> Sampler::Create(const SamplerSpecification& spec)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return std::make_shared<VulkanSampler>(spec); break;
		}

		LY_CORE_ASSERT(false, "Unknown Graphics API");
		return nullptr;
	}
}
