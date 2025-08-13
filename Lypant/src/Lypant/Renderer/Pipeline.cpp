#include <lypch.h>
#include "Pipeline.h"
#include "Vulkan/VulkanPipeline.h"
#include "Vulkan/VulkanGraphicsContext.h"
#include "Vulkan/VulkanShader.h"

namespace lypant
{
	std::shared_ptr<GraphicsPipeline> GraphicsPipeline::Create(const GraphicsPipelineSpecification& spec, const std::shared_ptr<Shader>& shader)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return std::make_shared<VulkanGraphicsPipeline>(spec, shader); break;
		}

		LY_CORE_ASSERT(false, "Unknown Graphics API");
		return nullptr;
	}
}
