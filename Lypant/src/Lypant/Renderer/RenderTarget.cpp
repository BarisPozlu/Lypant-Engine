#include <lypch.h>
#include "RenderTarget.h"
#include "GraphicsContext.h"
#include "Vulkan/VulkanGraphicsContext.h"
#include "Vulkan/VulkanSwapChain.h"
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

	std::shared_ptr<RenderTarget> RenderTarget::GetDefault()
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return VulkanGraphicsContext::Get().GetSwapChain().GetDefaultRenderTarget(); break;
		}

		LY_CORE_ASSERT(false, "Unknown Graphics API");
		return nullptr;
	}
}