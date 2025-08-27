#include <lypch.h>
#include "VulkanPipeline.h"
#include "VulkanGraphicsContext.h"

namespace lypant
{
	VulkanGraphicsPipeline::VulkanGraphicsPipeline(const GraphicsPipelineSpecification& spec, const std::shared_ptr<Shader>& shader, const std::shared_ptr<RenderTarget>& renderTarget)
	{
		const auto& vulkanShader = reinterpret_cast<const std::shared_ptr<VulkanShader>&>(shader);
		const auto& vulkanRenderTarget = reinterpret_cast<const std::shared_ptr<VulkanRenderTarget>&>(renderTarget);

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		std::array<VkDynamicState, 2> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
		dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicStateInfo.pDynamicStates = dynamicStates.data();

		VkPipelineViewportStateCreateInfo viewportStateInfo{};
		viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateInfo.viewportCount = 1;
		viewportStateInfo.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizerInfo{};
		rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizerInfo.depthClampEnable = VK_FALSE;
		rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizerInfo.lineWidth = 1.0f;
		rasterizerInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizerInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizerInfo.depthBiasEnable = VK_FALSE;

		// TODO: Make this modifiable
		VkPipelineMultisampleStateCreateInfo multisamplingInfo{};
		multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisamplingInfo.sampleShadingEnable = VK_FALSE;
		multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		// TODO: Make this modifiable
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
		colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendInfo.logicOpEnable = VK_FALSE;
		colorBlendInfo.attachmentCount = 1;
		colorBlendInfo.pAttachments = &colorBlendAttachment;

		// TODO: Make this modifiable
		VkPipelineRenderingCreateInfoKHR renderingInfo{};
		renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
		renderingInfo.colorAttachmentCount = 1;
		VkFormat format = vulkanRenderTarget->GetColorBuffer()->GetFormat();
		renderingInfo.pColorAttachmentFormats = &format;

		VkGraphicsPipelineCreateInfo graphicsPipelineInfo{};
		graphicsPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		graphicsPipelineInfo.pNext = &renderingInfo;
		graphicsPipelineInfo.stageCount = 2;
		graphicsPipelineInfo.pStages = vulkanShader->GetShaderStageInfos().data();
		graphicsPipelineInfo.pVertexInputState = &vertexInputInfo;
		graphicsPipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
		graphicsPipelineInfo.pViewportState = &viewportStateInfo;
		graphicsPipelineInfo.pRasterizationState = &rasterizerInfo;
		graphicsPipelineInfo.pMultisampleState = &multisamplingInfo;
		graphicsPipelineInfo.pColorBlendState = &colorBlendInfo;
		graphicsPipelineInfo.pDynamicState = &dynamicStateInfo;
		graphicsPipelineInfo.layout = vulkanShader->GetPipelineLayout();

		vkCreateGraphicsPipelines(VulkanGraphicsContext::Get().GetDevice(), VK_NULL_HANDLE, 1, &graphicsPipelineInfo, nullptr, &m_GraphicsPipeline);
	}

	VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
	{
		vkDestroyPipeline(VulkanGraphicsContext::Get().GetDevice(), m_GraphicsPipeline, nullptr);
	}
}
