#include <lypch.h>
#include "Shader.h"
#include "Vulkan/VulkanGraphicsContext.h"
#include "Vulkan/VulkanShader.h"

namespace lypant
{
	std::shared_ptr<Shader> Shader::Create(const std::string& path)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return std::make_shared<VulkanShader>(path); break;
		}

		LY_CORE_ASSERT(false, "Unknown Graphics API");
		return nullptr;
	}
}
