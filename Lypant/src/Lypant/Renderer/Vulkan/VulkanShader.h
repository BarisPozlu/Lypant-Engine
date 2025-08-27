#pragma once

#include <vulkan/vulkan.h>
#include <Lypant/Renderer/Shader.h>

namespace lypant
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::string& path);
		virtual ~VulkanShader();
		inline const std::array<VkPipelineShaderStageCreateInfo, 2>& GetShaderStageInfos() const { return m_ShaderStageInfos; }
		inline VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }
		inline const std::unordered_map<uint32_t, VkDescriptorSetLayout>& GetDescriptorSetLayouts() const { return m_DescriptorSetLayouts; }
		inline VkDescriptorSetLayout GetDescriptorSetLayout(uint32_t set) const { return m_DescriptorSetLayouts.at(set); }
	private:
		void Reflect(const std::vector<uint32_t>& shaderCode);
	private:
		VkShaderModule m_ShaderModules[2]{};
		std::array<VkPipelineShaderStageCreateInfo, 2> m_ShaderStageInfos{};
		std::unordered_map<uint32_t, VkDescriptorSetLayout> m_DescriptorSetLayouts;
		VkPipelineLayout m_PipelineLayout;
	};
}