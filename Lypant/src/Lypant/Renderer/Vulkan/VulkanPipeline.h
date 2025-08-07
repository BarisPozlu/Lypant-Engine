#pragma once

#include <memory>
#include <vulkan/vulkan.h>
#include "VulkanShader.h"

namespace lypant
{
	struct GraphicsPipelineSpecification
	{

	};


	class VulkanGraphicsPipeline
	{
	public:
		VulkanGraphicsPipeline(const GraphicsPipelineSpecification& spec, const std::shared_ptr<VulkanShader>& shader);
		virtual ~VulkanGraphicsPipeline();
		inline VkPipeline Get() const { return m_GraphicsPipeline; }
	private:
		VkPipeline m_GraphicsPipeline;
	};
}
