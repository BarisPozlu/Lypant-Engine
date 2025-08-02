#include <lypch.h>
#include "VulkanShader.h"
#include "VulkanGraphicsContext.h"
#include <shaderc/shaderc.hpp>

namespace lypant
{
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

    static std::vector<uint32_t> CompileGlslToSpv(const std::string& path, const std::string& macro)
    {
        std::string source = ReadFile(path);

        shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        options.SetOptimizationLevel(shaderc_optimization_level_performance);
        options.AddMacroDefinition(macro);

        shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(source, shaderc_vertex_shader, path.data(), options);

        #ifdef LYPANT_DEBUG
        if (result.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            LY_CORE_ERROR(result.GetErrorMessage());
            LY_CORE_ASSERT(false, "Vertex shader could not compile");
            return std::vector<uint32_t>();
        }
        #endif

        return { result.cbegin(), result.cend() };
    }

	VulkanShader::VulkanShader(const std::string& path)
	{
        auto& graphicsContext = VulkanGraphicsContext::Get();

        std::vector<uint32_t> vertexShaderCode = CompileGlslToSpv(path, "VERTEX_SHADER");
        std::vector<uint32_t> fragmentShaderCode = CompileGlslToSpv(path, "FRAGMENT_SHADER");

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
	}

	VulkanShader::~VulkanShader()
	{
        for (VkShaderModule shaderModule : m_ShaderModules)
        {
            vkDestroyShaderModule(VulkanGraphicsContext::Get().GetDevice(), shaderModule, nullptr);
        }
	}
}
