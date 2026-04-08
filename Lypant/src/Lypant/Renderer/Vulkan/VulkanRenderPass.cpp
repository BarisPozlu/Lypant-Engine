#include <lypch.h>
#include "VulkanRenderPass.h"
#include "VulkanGraphicsContext.h"

namespace lypant
{
	VulkanSubpass::VulkanSubpass(const std::shared_ptr<RenderTarget>& renderTarget, const RenderTargetOperation& op, const std::shared_ptr<Shader>& shader, const std::vector<ImageBinding>& imageBindings, uint32_t uniformBufferSize, const void* data, bool isDynamic, int subpassFlags)
	{
		m_RenderTarget = reinterpret_cast<const std::shared_ptr<VulkanRenderTarget>&>(renderTarget);
		m_Shader = reinterpret_cast<const std::shared_ptr<VulkanShader>&>(shader);
		m_DataBindings = imageBindings;

		VkDescriptorSetLayout layout = m_Shader->GetDescriptorSetLayout(DescriptorSetType::RenderPass);

		if (layout != VK_NULL_HANDLE)
		{
			m_DescriptorSet = std::make_shared<VulkanDescriptorSet>(layout);

			if (uniformBufferSize)
			{
				m_UniformBuffer = reinterpret_cast<const std::shared_ptr<VulkanBuffer>&>(Buffer::CreateUniformBuffer(uniformBufferSize, data, isDynamic));
				m_DescriptorSet->Update(imageBindings, { { m_UniformBuffer, static_cast<uint32_t>(imageBindings.size()), 0, m_UniformBuffer->GetSize()}});
			}

			else
			{
				m_DescriptorSet->Update(imageBindings, { });
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
