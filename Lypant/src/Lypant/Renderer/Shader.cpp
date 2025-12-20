#include <lypch.h>
#include "Shader.h"
#include "Vulkan/VulkanGraphicsContext.h"
#include "Vulkan/VulkanShader.h"

namespace lypant
{
	std::shared_ptr<Shader> Shader::Create(const std::string& path)
	{
		const auto it = s_Cache.find(path);
		if (it != s_Cache.end())
		{
			return std::shared_ptr<Shader>(it->second);
		}

		std::shared_ptr<Shader> shader;

		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: shader = std::make_shared<VulkanShader>(path); break;
			default: LY_CORE_ASSERT(false, "Unknown Graphics API");
		}

		s_Cache[path] = std::weak_ptr<Shader>(shader);
		return shader;
	}
}
