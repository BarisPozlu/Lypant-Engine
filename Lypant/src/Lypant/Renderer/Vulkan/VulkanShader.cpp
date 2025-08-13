#include <lypch.h>
#include "VulkanShader.h"
#include "VulkanGraphicsContext.h"
#include <shaderc/shaderc.hpp>
#include <spirv_reflect.h>

namespace lypant
{
    static std::unordered_map<uint32_t, VkPushConstantRange> s_PushConstantMap;

    static std::string ReadFile(const std::string& path)
    {
        std::string source;

        std::ifstream in(path, std::ios::in | std::ios::binary | std::ios::ate);
        LY_CORE_ASSERT(in, "Could not open file!");

        source.resize(in.tellg());

        in.seekg(std::ios::beg);
        in.read(source.data(), source.size());

        in.close();
        return source;
    }

    static std::vector<uint32_t> CompileGlslToSpv(const std::string& path, shaderc_shader_kind type)
    {
        std::string source = ReadFile(path);

        shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        options.SetOptimizationLevel(shaderc_optimization_level_performance);

        switch (type)
        {
            case shaderc_vertex_shader: options.AddMacroDefinition("VERTEX_SHADER"); break;
            case shaderc_fragment_shader: options.AddMacroDefinition("FRAGMENT_SHADER"); break;
            default: LY_CORE_ASSERT(false, "Shader type not supported");
        }
        
        shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(source, type, path.data(), options);

        #ifdef LYPANT_DEBUG
        if (result.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            LY_CORE_ERROR(result.GetErrorMessage());
            LY_CORE_ASSERT(false, "Shader could not compile");
            return std::vector<uint32_t>();
        }
        #endif

        return { result.cbegin(), result.cend() };
    }

	VulkanShader::VulkanShader(const std::string& path)
	{
        auto& graphicsContext = VulkanGraphicsContext::Get();

        std::vector<uint32_t> vertexShaderCode = CompileGlslToSpv(path, shaderc_vertex_shader);
        std::vector<uint32_t> fragmentShaderCode = CompileGlslToSpv(path, shaderc_fragment_shader);

        VkShaderModuleCreateInfo moduleInfo{};
        moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        moduleInfo.codeSize = vertexShaderCode.size() * sizeof(uint32_t);
        moduleInfo.pCode = vertexShaderCode.data();

        vkCreateShaderModule(graphicsContext.GetDevice(), &moduleInfo, nullptr, &m_ShaderModules[0]);

        moduleInfo.codeSize = fragmentShaderCode.size() * sizeof(uint32_t);
        moduleInfo.pCode = fragmentShaderCode.data();

        vkCreateShaderModule(graphicsContext.GetDevice(), &moduleInfo, nullptr, &m_ShaderModules[1]);

        m_ShaderStageInfos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        m_ShaderStageInfos[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        m_ShaderStageInfos[0].module = m_ShaderModules[0];
        m_ShaderStageInfos[0].pName = "main";

        m_ShaderStageInfos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        m_ShaderStageInfos[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        m_ShaderStageInfos[1].module = m_ShaderModules[1];
        m_ShaderStageInfos[1].pName = "main";

        Reflect(vertexShaderCode);
        Reflect(fragmentShaderCode);

        VkPipelineLayoutCreateInfo pipelineLayout{};
        pipelineLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayout.setLayoutCount = m_DescriptorSetLayouts.size();
        pipelineLayout.pSetLayouts = m_DescriptorSetLayouts.data();
        pipelineLayout.pushConstantRangeCount = s_PushConstantMap.size();
        
        std::vector<VkPushConstantRange> ranges;
        ranges.reserve(s_PushConstantMap.size());
        for (auto& [key, value] : s_PushConstantMap)
        {
            ranges.push_back(value);
        }

        pipelineLayout.pPushConstantRanges = ranges.data();
        vkCreatePipelineLayout(graphicsContext.GetDevice(), &pipelineLayout, nullptr, &m_PipelineLayout);

        s_PushConstantMap.clear();
	}

	VulkanShader::~VulkanShader()
	{
        auto& graphicsContext = VulkanGraphicsContext::Get();

        vkDestroyPipelineLayout(graphicsContext.GetDevice(), m_PipelineLayout, nullptr);

        for (VkDescriptorSetLayout layout : m_DescriptorSetLayouts)
        {
            vkDestroyDescriptorSetLayout(graphicsContext.GetDevice(), layout, nullptr);
        }

        for (VkShaderModule shaderModule : m_ShaderModules)
        {
            vkDestroyShaderModule(graphicsContext.GetDevice(), shaderModule, nullptr);
        }
	}

    void VulkanShader::Reflect(const std::vector<uint32_t>& shaderCode)
    {
        // Reflect descriptor sets
        SpvReflectShaderModule reflModule{};
        spvReflectCreateShaderModule(shaderCode.size() * sizeof(uint32_t), shaderCode.data(), &reflModule);

        uint32_t count = 0;
        spvReflectEnumerateDescriptorSets(&reflModule, &count, nullptr);

        std::vector<SpvReflectDescriptorSet*> reflSets(count);
        spvReflectEnumerateDescriptorSets(&reflModule, &count, reflSets.data());

        m_DescriptorSetLayouts.resize(count);
        std::vector<VkDescriptorSetLayoutBinding> bindings;

        for (int i = 0; i < reflSets.size(); i++)
        {
            const SpvReflectDescriptorSet& reflSet = *reflSets[i];

            bindings.resize(reflSet.binding_count);

            for (int j = 0; j < reflSet.binding_count; j++)
            {
                const SpvReflectDescriptorBinding& reflBinding = *reflSet.bindings[j];
                VkDescriptorSetLayoutBinding& binding = bindings[j];

                binding.binding = reflBinding.binding;
                binding.stageFlags = static_cast<VkShaderStageFlagBits>(reflModule.shader_stage);
                binding.descriptorType = static_cast<VkDescriptorType>(reflBinding.descriptor_type);

                binding.descriptorCount = 1;
                for (uint32_t dimension = 0; dimension < reflBinding.array.dims_count; dimension++)
                {
                    binding.descriptorCount *= reflBinding.array.dims[dimension];
                }

                VkDescriptorSetLayoutCreateInfo createInfo{};
                createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                createInfo.bindingCount = bindings.size();
                createInfo.pBindings = bindings.data();

                vkCreateDescriptorSetLayout(VulkanGraphicsContext::Get().GetDevice(), &createInfo, nullptr, &m_DescriptorSetLayouts[i]);
            }
        }

        // Reflect push constants
        spvReflectEnumeratePushConstantBlocks(&reflModule, &count, nullptr);

        std::vector<SpvReflectBlockVariable*> reflPushConstants(count);

        spvReflectEnumeratePushConstantBlocks(&reflModule, &count, reflPushConstants.data());

        for (int i = 0; i < count; i++)
        {
            auto it = s_PushConstantMap.find(reflPushConstants[i]->spirv_id);

            if (it == s_PushConstantMap.end())
            {
                VkPushConstantRange range{};
                range.offset = reflPushConstants[i]->offset;
                range.size = reflPushConstants[i]->size;
                range.stageFlags = reflModule.shader_stage;
                s_PushConstantMap[reflPushConstants[i]->spirv_id] = range;
            }

            else
            {
                it->second.stageFlags |= reflModule.shader_stage;
            }

        }
    }
}
