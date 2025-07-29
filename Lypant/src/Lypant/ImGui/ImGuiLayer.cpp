#include "lypch.h"
#include "ImGuiLayer.h"
#include <Lypant/Renderer/GraphicsContext.h>
#include "VulkanImGuiLayer.h"

namespace lypant
{
	ImGuiLayer* ImGuiLayer::Create()
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return new VulkanImGuiLayer(); break;
		}

		LY_CORE_ASSERT(false, "Unknown Graphics API");
		return nullptr;
	}
}
