#pragma once

#include <Lypant/Renderer/Material.h>
#include <vulkan/vulkan.h>
#include "VulkanShader.h"
#include "VulkanDescriptorSet.h"
#include "VulkanImage.h"

namespace lypant
{
	class VulkanMaterial : public Material
	{
	public:
		VulkanMaterial(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Image>& albedo);
		virtual ~VulkanMaterial();
	private:
		void UpdateMaterial();
	private:
		std::shared_ptr<VulkanShader> m_Shader;
		std::shared_ptr<VulkanDescriptorSet> m_DescriptorSet;
		std::shared_ptr<VulkanImage> m_Albedo;
	};
}
