#pragma once

#include <memory>
#include <vulkan/vulkan.h>
#include <Lypant/Renderer/Pipeline.h>
#include "VulkanShader.h"

namespace lypant
{
	class VulkanGraphicsPipeline : public GraphicsPipeline
	{
	public:
		VulkanGraphicsPipeline(const GraphicsPipelineSpecification& spec, const std::shared_ptr<Shader>& shader);
		virtual ~VulkanGraphicsPipeline();
		inline VkPipeline GetVkPipeline() const { return m_GraphicsPipeline; }
	private:
		VkPipeline m_GraphicsPipeline;
	};
}
