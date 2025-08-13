#include <lypch.h>
#include "RenderPass.h"
#include "GraphicsContext.h"
#include "Vulkan/VulkanRenderPass.h"

namespace lypant
{
	std::unique_ptr<Subpass> Subpass::Create(const std::shared_ptr<RenderTarget>& renderTarget, const std::shared_ptr<Shader>& shader, const std::vector<DataBinding>& dataBindings)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return std::make_unique<VulkanSubpass>(renderTarget, shader, dataBindings); break;
		}

		LY_CORE_ASSERT(false, "Unknown Graphics API");
		return nullptr;
	}

	//std::unique_ptr<RenderPass> RenderPass::Create(const std::vector<Subpass>& subpasses)
	//{
	//	switch (GraphicsContext::GetGraphicsAPI())
	//	{
	//		case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
	//		case GraphicsAPI::Vulkan: return std::make_unique<VulkanRenderPass>(subpasses); break;
	//	}

	//	LY_CORE_ASSERT(false, "Unknown Graphics API");
	//	return nullptr;
	//}
}
