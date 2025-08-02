#pragma once

#include <vulkan/vulkan.h>

namespace lypant
{
	class VulkanShader
	{
	public:
		VulkanShader(const std::string& path);
		virtual ~VulkanShader();
		inline const std::array<VkPipelineShaderStageCreateInfo, 2>& GetShaderStageInfos() const { return m_ShaderStageInfos; }
	private:
		VkShaderModule m_ShaderModules[2]{};
		std::array<VkPipelineShaderStageCreateInfo, 2> m_ShaderStageInfos{};
	};
}