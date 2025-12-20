#include <lypch.h>
#include "Material.h"
#include "GraphicsContext.h"
#include "Vulkan/VulkanMaterial.h"

namespace lypant
{
	std::shared_ptr<Material> Material::Create(const MaterialData& data)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return std::make_shared<VulkanMaterial>(data); break;
		}

		LY_CORE_ASSERT(false, "Unknown Graphics API");
		return nullptr;
	}
}