#include <lypch.h>
#include "VulkanShader.h"
#include "VulkanGraphicsContext.h"
#include <shaderc/shaderc.hpp>
#include <spirv_reflect.h>

namespace lypant
{
    static std::unordered_map<uint32_t, VkPushConstantRange> s_PushConstantMap;
    static std::unordered_map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> s_DescriptorSetMap;

    VkShaderStageFlags GetVkShaderStageFlags(int shaderStageFlags)
    {
        VkShaderStageFlags flags = 0;

        if (shaderStageFlags & ShaderStageFlagsVertex)
        {
            flags |= VK_SHADER_STAGE_VERTEX_BIT;
        }

        if (shaderStageFlags & ShaderStageFlagsGeometry)
        {
            flags |= VK_SHADER_STAGE_GEOMETRY_BIT;
        }

        if (shaderStageFlags & ShaderStageFlagsFragment)
        {
            flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
        }

        return flags;
    }

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

        // NOTE: This is needed so that names are also in the spirv binary
        options.SetGenerateDebugInfo();

        //TODO: options.SetOptimizationLevel(shaderc_optimization_level_performance);
        options.SetOptimizationLevel(shaderc_optimization_level_zero);

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

    static VkDescriptorType GetDescriptorTypeFromReflBinding(const SpvReflectDescriptorBinding& reflBinding)
    {
        if (reflBinding.type_description->type_name)
        {
            std::string name = reflBinding.type_description->type_name;
            if (name.find("Dynamic") != std::string::npos)
            {
                switch (reflBinding.descriptor_type)
                {
                    case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
                    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
                }

                LY_CORE_ASSERT(false, "Binding is named to be dynamic but is not a uniform or a storage buffer");
                return VK_DESCRIPTOR_TYPE_MAX_ENUM;
            }
        }

        return static_cast<VkDescriptorType>(reflBinding.descriptor_type);
    }

	VulkanShader::VulkanShader(const std::string& path) : m_Path(path)
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

        m_DescriptorSetLayouts.reserve(s_DescriptorSetMap.size() + 1);
        m_DescriptorSetLayouts[0] = graphicsContext.GetGlobalDescriptorSetLayout();

        for (auto& [key, value] : s_DescriptorSetMap)
        {
            VkDescriptorSetLayoutCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            createInfo.bindingCount = value.size();
            createInfo.pBindings = value.data();

            vkCreateDescriptorSetLayout(VulkanGraphicsContext::Get().GetDevice(), &createInfo, nullptr, &m_DescriptorSetLayouts[key]);
        }

        VkPipelineLayoutCreateInfo pipelineLayout{};
        pipelineLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayout.setLayoutCount = m_DescriptorSetLayouts.size();
        pipelineLayout.pushConstantRangeCount = s_PushConstantMap.size();
        
        std::vector<VkDescriptorSetLayout> layouts;
        layouts.reserve(m_DescriptorSetLayouts.size());
        for (auto& [key, value] : m_DescriptorSetLayouts)
        {
            layouts.push_back(value);
        }

        pipelineLayout.pSetLayouts = layouts.data();

        std::vector<VkPushConstantRange> ranges;
        ranges.reserve(s_PushConstantMap.size());
        for (auto& [key, value] : s_PushConstantMap)
        {
            ranges.push_back(value);
        }

        pipelineLayout.pPushConstantRanges = ranges.data();

        vkCreatePipelineLayout(graphicsContext.GetDevice(), &pipelineLayout, nullptr, &m_PipelineLayout);

        s_PushConstantMap.clear();
        s_DescriptorSetMap.clear();
	}

	VulkanShader::~VulkanShader()
	{
        s_Cache.erase(m_Path);

        auto& graphicsContext = VulkanGraphicsContext::Get();

        VkPipelineLayout pipelineLayout = m_PipelineLayout;
        auto& shaderModules = m_ShaderModules;

        // NOTE: Set 0 layout is not owned by the shader, it can't destroy it.
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
        descriptorSetLayouts.reserve(m_DescriptorSetLayouts.size() - 1);
        for (auto& [key, value] : m_DescriptorSetLayouts)
        {
            if (key == 0) continue;
            descriptorSetLayouts.push_back(value);
        }

        graphicsContext.GetDeletionQueue().PushFunction([pipelineLayout, descriptorSetLayouts, shaderModules]()
            {
                auto& graphicsContext = VulkanGraphicsContext::Get();

                vkDestroyPipelineLayout(graphicsContext.GetDevice(), pipelineLayout, nullptr);

                for (auto layout : descriptorSetLayouts)
                {
                    vkDestroyDescriptorSetLayout(graphicsContext.GetDevice(), layout, nullptr);
                }

                for (VkShaderModule shaderModule : shaderModules)
                {
                    vkDestroyShaderModule(graphicsContext.GetDevice(), shaderModule, nullptr);
                }
            });
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

        for (int i = 0; i < reflSets.size(); i++)
        {
            const SpvReflectDescriptorSet& reflSet = *reflSets[i];
            
            // NOTE: Set 0 is global and added to every single pipeline layout no need to reflect here.
            if (reflSet.set == 0) continue;
                 
            for (int j = 0; j < reflSet.binding_count; j++)
            {
                const SpvReflectDescriptorBinding& reflBinding = *reflSet.bindings[j];

                auto& bindings = s_DescriptorSetMap[reflSet.set];

                // NOTE: We might have the same binding already in the bindings if that's the case we simply add this shader stage to the flags and continue
                bool bindingFound = false;
                for (auto& binding : bindings)
                {
                    if (binding.binding == reflBinding.binding)
                    {
                        binding.stageFlags |= static_cast<VkShaderStageFlagBits>(reflModule.shader_stage);
                        bindingFound = true;
                        break;
                    }
                }
                if (bindingFound) continue;

                bindings.resize(bindings.size() + 1);

                VkDescriptorSetLayoutBinding& binding = bindings.back();

                binding.binding = reflBinding.binding;
                binding.stageFlags = static_cast<VkShaderStageFlagBits>(reflModule.shader_stage);
                binding.descriptorType = GetDescriptorTypeFromReflBinding(reflBinding);

                binding.descriptorCount = 1;
                for (uint32_t dimension = 0; dimension < reflBinding.array.dims_count; dimension++)
                {
                    binding.descriptorCount *= reflBinding.array.dims[dimension];
                }
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
