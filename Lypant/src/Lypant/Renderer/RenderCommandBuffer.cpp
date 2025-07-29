#include <lypch.h>
#include "RenderCommandBuffer.h"
#include "Vulkan/VulkanRenderCommandBuffer.h"
#include "GraphicsContext.h"

namespace lypant
{
	std::unique_ptr<RenderCommandBuffer> RenderCommandBuffer::Create()
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return std::make_unique<VulkanRenderCommandBuffer>(); break;
		}

		LY_CORE_ASSERT(false, "Unknown Graphics API");
		return nullptr;
		
	}
}
