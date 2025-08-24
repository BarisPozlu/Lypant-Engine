#include <lypch.h>
#include "VulkanMaterial.h"

namespace lypant
{
	VulkanMaterial::VulkanMaterial(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Image>& albedo)
	{
		m_Shader = reinterpret_cast<const std::shared_ptr<VulkanShader>&>(shader);
		m_Albedo = reinterpret_cast<const std::shared_ptr<VulkanImage>&>(albedo);

		m_DescriptorSet = std::make_shared<VulkanDescriptorSet>(m_Shader->GetDescriptorSetLayout(0));

		UpdateMaterial();
	}

	VulkanMaterial::~VulkanMaterial()
	{

	}

	void VulkanMaterial::UpdateMaterial()
	{
		m_DescriptorSet->Update({ { m_Albedo, 0 } });
	}
}