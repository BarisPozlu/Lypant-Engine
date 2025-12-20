#include <lypch.h>
#include "RenderPass.h"
#include "GraphicsContext.h"
#include "Vulkan/VulkanRenderPass.h"

namespace lypant
{
	std::unique_ptr<Subpass> Subpass::Create(const std::shared_ptr<RenderTarget>& renderTarget, const RenderTargetOperation& op, const std::shared_ptr<Shader>& shader, const std::vector<ImageBinding>& dataBindings, uint32_t uniformBufferSize, const void* data, bool isDynamic, int subpassFlags)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return std::make_unique<VulkanSubpass>(renderTarget, op, shader, dataBindings, uniformBufferSize, data, isDynamic, subpassFlags); break;
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
