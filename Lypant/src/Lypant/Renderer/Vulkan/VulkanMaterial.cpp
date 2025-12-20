#include <lypch.h>
#include "VulkanMaterial.h"
#include "VulkanShader.h"
#include "VulkanImage.h"

namespace lypant
{
	VulkanMaterial::VulkanMaterial(const MaterialData& data) : m_Data(data)
	{
		auto& shader = reinterpret_cast<std::shared_ptr<VulkanShader>&>(Shader::Create("shaders/Model_PBR.glsl"));

		m_DescriptorSet = std::make_shared<VulkanDescriptorSet>(shader->GetDescriptorSetLayout(2));

		UpdateMaterial();
	}

	VulkanMaterial::~VulkanMaterial()
	{

	}

	void VulkanMaterial::UpdateMaterial()
	{
		std::vector<ImageBinding> bindings;
		bindings.reserve(6);

		bindings.push_back({ m_Data.Textures.AlbedoMap, 0 });
		bindings.push_back({ m_Data.Textures.ORMMap, 1 });
		// TODO: Binding 2 is not seen by the reflection library because it is being compiled out when optimaztion is on
		bindings.push_back({ m_Data.Textures.AmbientOcclusionMap, 2 });
		bindings.push_back({ m_Data.Textures.RoughnessMap, 3 });
		bindings.push_back({ m_Data.Textures.MetallicMap, 4 });
		bindings.push_back({ m_Data.Textures.NormalMap, 5 });

		m_DescriptorSet->Update(bindings, { });
	}
}
