#include "lypch.h"
#include "Material.h"

namespace lypant
{
	Material::Material(const std::string& shaderPath, const std::string& albedoMapPath, const std::string& AORoughnessMetallicMapPath, const std::string& normalMapPath)
	{
		m_Shader = Shader::Load(shaderPath);
		m_AlbedoMap = Texture2D::Load(albedoMapPath, false);
		m_AORoughnessMetallicMap = Texture2D::Load(AORoughnessMetallicMapPath);
		m_NormalMap = Texture2D::Load(normalMapPath);
		m_Buffer = nullptr;
	}

	Material::Material(const std::string& shaderPath, const std::shared_ptr<Texture2D>& albedoMap, const std::shared_ptr<Texture2D>& aoRoughnessMetallicMap, const std::shared_ptr<Texture2D>& normalMap)
	{
		m_Shader = Shader::Load(shaderPath);
		m_AlbedoMap = albedoMap;
		m_AORoughnessMetallicMap = aoRoughnessMetallicMap;
		m_NormalMap = normalMap;
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
					if (name.find("Albedo") != std::string::npos)
					{
						LY_CORE_ASSERT(m_AlbedoMap, "Albedo map is nullptr despite the shader having a Albedo map.")
						m_AlbedoMap->Bind(m_Shader->GetUniformValueInt(name));
					}
					
					else if (name.find("AORoughnessMetallic") != std::string::npos)
					{
						LY_CORE_ASSERT(m_AORoughnessMetallicMap, "AORoughnessMetallic map is nullptr despite the shader having a AORoughnessMetallic map.")
						m_AORoughnessMetallicMap->Bind(m_Shader->GetUniformValueInt(name));
					}

					else if (name.find("Normal") != std::string::npos)
					{
						LY_CORE_ASSERT(m_NormalMap, "Normal map is nullptr despite the shader having a Normal map.")
						m_NormalMap->Bind(m_Shader->GetUniformValueInt(name));
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
