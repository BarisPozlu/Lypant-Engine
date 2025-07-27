#include <lypch.h>
#include "Renderer.h"
#include "Vulkan/VulkanRenderer.h"

namespace lypant
{
	Renderer* Renderer::s_Instance = nullptr;
	GraphicsAPI Renderer::s_GraphicsAPI = GraphicsAPI::None;

	std::unique_ptr<Renderer> Renderer::Create()
	{
		switch (Renderer::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return std::make_unique<VulkanRenderer>(); break;
		}

		LY_CORE_ASSERT(false, "Unknown Graphics API");
		return nullptr;
		
	}

	Renderer::Renderer()
	{
		LY_CORE_ASSERT(!s_Instance, "There is already a renderer created");
		s_Instance = this;
	}
}
