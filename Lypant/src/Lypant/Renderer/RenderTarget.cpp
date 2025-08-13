#include <lypch.h>
#include "RenderTarget.h"
#include "GraphicsContext.h"
#include "Vulkan/VulkanRenderTarget.h"

namespace lypant
{
	std::shared_ptr<RenderTarget> RenderTarget::Create()
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return std::make_shared<VulkanRenderTarget>(); break;
		}

		LY_CORE_ASSERT(false, "Unknown Graphics API");
		return nullptr;
	}

	std::shared_ptr<RenderTarget> RenderTarget::CreateDefault()
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return std::make_shared<VulkanDefaultRenderTarget>(); break;
		}

		LY_CORE_ASSERT(false, "Unknown Graphics API");
		return nullptr;
	}
}