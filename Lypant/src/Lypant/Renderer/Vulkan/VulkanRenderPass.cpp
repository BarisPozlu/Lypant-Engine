#include <lypch.h>
#include "VulkanRenderPass.h"
#include "VulkanGraphicsContext.h"

namespace lypant
{
	VulkanSubpass::VulkanSubpass(const std::shared_ptr<RenderTarget>& renderTarget, const std::shared_ptr<Shader>& shader, const std::vector<DataBinding>& dataBindings)
	{
		m_RenderTarget = reinterpret_cast<const std::shared_ptr<VulkanRenderTarget>&>(renderTarget);
		m_Shader = reinterpret_cast<const std::shared_ptr<VulkanShader>&>(shader);
		
		// TODO: Update
		if (m_Shader->GetDescriptorSetLayouts().size())
		{
			m_DescriptorSet = std::make_shared<VulkanDescriptorSet>(m_Shader->GetDescriptorSetLayouts()[0]);
			m_DescriptorSet->Update(dataBindings);
		}
		

		m_GraphicsPipeline = reinterpret_cast<const std::shared_ptr<VulkanGraphicsPipeline>&>(GraphicsPipeline::Create(GraphicsPipelineSpecification(), m_Shader));
	}
}
