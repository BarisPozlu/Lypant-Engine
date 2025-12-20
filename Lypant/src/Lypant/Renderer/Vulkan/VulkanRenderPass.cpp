#include <lypch.h>
#include "VulkanRenderPass.h"
#include "VulkanGraphicsContext.h"

namespace lypant
{
	VulkanSubpass::VulkanSubpass(const std::shared_ptr<RenderTarget>& renderTarget, const RenderTargetOperation& op, const std::shared_ptr<Shader>& shader, const std::vector<ImageBinding>& dataBindings, uint32_t uniformBufferSize, const void* data, bool isDynamic, int subpassFlags)
	{
		m_RenderTarget = reinterpret_cast<const std::shared_ptr<VulkanRenderTarget>&>(renderTarget);
		m_Shader = reinterpret_cast<const std::shared_ptr<VulkanShader>&>(shader);
		m_DataBindings = dataBindings;

		// TODO: Update
		if (uniformBufferSize)
		{
			m_UniformBuffer = reinterpret_cast<const std::shared_ptr<VulkanBuffer>&>(Buffer::CreateUniformBuffer(uniformBufferSize, data, isDynamic));

			//if (data)
			//{
			//	m_UniformBuffer->UploadData(data, uniformBufferSize, 0);
			//}
		}
		if (m_Shader->GetDescriptorSetLayouts().size() > 1)
		{
			m_DescriptorSet = std::make_shared<VulkanDescriptorSet>(m_Shader->GetDescriptorSetLayout(1));

			if (uniformBufferSize)
			{
				m_DescriptorSet->Update(dataBindings, { { m_UniformBuffer, static_cast<uint32_t>(dataBindings.size()), 0, m_UniformBuffer->GetSize()}});
			}
			else
			{
				m_DescriptorSet->Update(dataBindings, { });
			}
		}

		m_GraphicsPipeline = reinterpret_cast<const std::shared_ptr<VulkanGraphicsPipeline>&>(GraphicsPipeline::Create(GraphicsPipelineSpecification(), m_Shader, renderTarget));
		m_SubpassFlags = subpassFlags;
		m_RenderTargetOperation = op;
	}

	void VulkanSubpass::UploadData(const void* data, uint32_t size, uint32_t offset)
	{
		m_UniformBuffer->UploadData(data, size, offset);
	}
}
