#include "lypch.h"
#include "Material.h"

namespace lypant
{
	Material::Material(const std::string& shaderPath, const std::string& diffuseMapPath, const std::string& specularMapPath)
	{
		m_Shader = Shader::Load(shaderPath);
		m_DiffuseMap = Texture2D::Load(diffuseMapPath, false);
		m_SpecularMap = Texture2D::Load(specularMapPath);
		m_Buffer = nullptr;
	}

	Material::Material(const std::string& shaderPath, const glm::vec3& color)
	{
		m_Shader = Shader::Load(shaderPath);

		int totalSize = 0;

		for (auto& [name, type] : m_Shader->GetUniformNamesToTypesMap()) // for now the buffer should only contain the color. 
		{
			if (type != ShaderDataType::Sampler2D)
			{
				totalSize += GetSizeFromShaderDataType(type);
			}
		}

		m_Buffer = new char[totalSize];
		memcpy(m_Buffer, &color, totalSize);
	}

	Material::~Material()
	{
		delete[] m_Buffer;
	}

	void Material::Bind() const
	{
		m_Shader->Bind();

		int offset = 0;

		for (auto& [name, type] : m_Shader->GetUniformNamesToTypesMap())
		{
			switch (type)
			{
				case ShaderDataType::Float3:
					LY_CORE_ASSERT(m_Buffer, "Material buffer is nullptr despite the shader having uniforms.");
					m_Shader->SetUniformVec3Float(name, (float*)&m_Buffer[offset]);
					offset += GetSizeFromShaderDataType(type);
					break;
				case ShaderDataType::Sampler2D:
					if (name.find("Diffuse") != std::string::npos)
					{
						LY_CORE_ASSERT(m_DiffuseMap.get(), "Diffuse map is nullptr despite the shader having a diffuse map.")
						m_DiffuseMap->Bind(m_Shader->GetUniformValueInt(name));
					}
					
					else if (name.find("Specular") != std::string::npos)
					{
						LY_CORE_ASSERT(m_SpecularMap.get(), "Specular map is nullptr despite the shader having a specular map.")
						m_SpecularMap->Bind(m_Shader->GetUniformValueInt(name));
					}
			}
		}
	}

	void Material::UpdateColor(const glm::vec3& color)
	{
		int offset = 0;

		for (auto& [name, type] : m_Shader->GetUniformNamesToTypesMap())
		{
			if (name.find("Color") == std::string::npos)
			{
				offset += GetSizeFromShaderDataType(type);
			}

			else
			{
				memcpy(&m_Buffer[offset], &color, sizeof(color));
				break;
			}
		}
	}
}
