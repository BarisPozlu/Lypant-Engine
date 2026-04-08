#pragma once

#include <Lypant/Renderer/Material.h>
#include <vulkan/vulkan.h>
#include "VulkanDescriptorSet.h"

namespace lypant
{
	class VulkanMaterial : public Material
	{
	public:
		VulkanMaterial(const MaterialData& data);
		virtual ~VulkanMaterial();
		virtual void UpdateMaterial() override;
		inline virtual const MaterialData& GetData() const override { return m_Data; }
		//inline const std::shared_ptr<VulkanDescriptorSet>& GetDescriptorSet() const { return m_DescriptorSet; }
	private:
		MaterialData m_Data;
		//std::shared_ptr<VulkanDescriptorSet> m_DescriptorSet;
	};
}
