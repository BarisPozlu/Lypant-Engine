#include "lypch.h"
#include "GraphicsContext.h"
#include "Vulkan/VulkanGraphicsContext.h"

namespace lypant
{
	GraphicsAPI GraphicsContext::s_GraphicsAPI = GraphicsAPI::None;

	GraphicsContext* GraphicsContext::Create(void* window)
	{
		switch (GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return new VulkanGraphicsContext(reinterpret_cast<GLFWwindow*>(window)); break;
		}

		LY_CORE_ASSERT(false, "Unknown Graphics API");
		return nullptr;
	}
}
