#include <lypch.h>
#include "Material.h"
#include "GraphicsContext.h"
#include "Vulkan/VulkanMaterial.h"

namespace lypant
{
	std::shared_ptr<Material> Material::Create(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Image>& albedo)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return std::make_shared<VulkanMaterial>(shader, albedo); break;
		}

		LY_CORE_ASSERT(false, "Unknown Graphics API");
		return nullptr;
	}
}