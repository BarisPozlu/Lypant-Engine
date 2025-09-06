#include <lypch.h>
#include "VulkanRenderPass.h"
#include "VulkanGraphicsContext.h"

namespace lypant
{
	VulkanSubpass::VulkanSubpass(const std::shared_ptr<RenderTarget>& renderTarget, bool shouldClear, const std::shared_ptr<Shader>& shader, const std::vector<DataBinding>& dataBindings, uint32_t uniformBufferSize, const void* data, bool isDynamic)
	{
		m_RenderTarget = reinterpret_cast<const std::shared_ptr<VulkanRenderTarget>&>(renderTarget);
		m_Shader = reinterpret_cast<const std::shared_ptr<VulkanShader>&>(shader);
		m_DataBindings = dataBindings;

		// TODO: Update
		if (uniformBufferSize)
		{
			m_UniformBuffer = reinterpret_cast<const std::shared_ptr<VulkanBuffer>&>(UniformBuffer::Create(uniformBufferSize, data, isDynamic));

			//if (data)
			//{
			//	m_UniformBuffer->UploadData(data, uniformBufferSize, 0);
			//}
		}
		if (m_Shader->GetDescriptorSetLayouts().size())
		{
			m_DescriptorSet = std::make_shared<VulkanDescriptorSet>(m_Shader->GetDescriptorSetLayout(1));
			m_DescriptorSet->Update(dataBindings, m_UniformBuffer);
		}

		m_GraphicsPipeline = reinterpret_cast<const std::shared_ptr<VulkanGraphicsPipeline>&>(GraphicsPipeline::Create(GraphicsPipelineSpecification(), m_Shader, renderTarget));
		m_ShouldClearTarget = shouldClear;
	}

	void VulkanSubpass::UploadData(const void* data, uint32_t size, uint32_t offset)
	{
		m_UniformBuffer->UploadData(data, size, offset);
	}
}
