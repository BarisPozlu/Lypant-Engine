#include "lypch.h"
#include "Material.h"

namespace lypant
{
	Material::Material(const std::string& shaderPath, const std::string& albedoMapPath, const std::string& ormMapPath, const std::string& normalMapPath)
	{
		m_Shader = Shader::Load(shaderPath);
		m_AlbedoMap = Texture2D::Load(albedoMapPath, false);
		m_ORMMap = Texture2D::Load(ormMapPath);
		m_NormalMap = Texture2D::Load(normalMapPath);
		m_Buffer = nullptr;
		m_UseCombinedORM = true;
	}

	Material::Material(const std::string& shaderPath, const std::string& albedoMapPath, const std::string& roughnessMapPath, const std::string& metallicMapPath, const std::string& normalMapPath, const std::string& aoMapPath)
	{
		m_Shader = Shader::Load(shaderPath);
		m_AlbedoMap = Texture2D::Load(albedoMapPath, false);
		if (aoMapPath.size())
		{
			m_AmbientOcclusionMap = Texture2D::Load(aoMapPath);
		}
		m_RoughnessMap = Texture2D::Load(roughnessMapPath);
		m_MetallicMap = Texture2D::Load(metallicMapPath);
		m_NormalMap = Texture2D::Load(normalMapPath);
		m_Buffer = nullptr;
		m_UseCombinedORM = false;
	}

	Material::Material(const std::string& shaderPath, const std::shared_ptr<Texture2D>& albedoMap, const std::shared_ptr<Texture2D>& ormMap, const std::shared_ptr<Texture2D>& normalMap)
	{
		m_Shader = Shader::Load(shaderPath);
		m_AlbedoMap = albedoMap;
		m_ORMMap = ormMap;
		m_NormalMap = normalMap;
		m_Buffer = nullptr;
		m_UseCombinedORM = true;
	}

	Material::Material(const std::string& shaderPath, const glm::vec3& color)
	{
		m_Shader = Shader::Load(shaderPath);

		//for (auto& [name, type] : m_Shader->GetUniformNamesToTypesMap()) // for now the buffer should only contain the color. 
		//{
		//	if (type != ShaderDataType::Sampler2D)
		//	{
		//		totalSize += GetSizeFromShaderDataType(type);
		//	}
		//}

		m_Buffer = new char[sizeof(glm::vec3)];
		memcpy(m_Buffer, &color, sizeof(glm::vec3));
	}

	Material::~Material()
	{
		delete[] m_Buffer;
	}

	void Material::Bind() const
	{
		m_Shader->Bind();

		m_Shader->SetUniformInt("u_UseCombinedORM", m_UseCombinedORM);

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

					else if (m_UseCombinedORM && name.find("ORM") != std::string::npos)
					{
						LY_CORE_ASSERT(m_ORMMap, "AORoughnessMetallic map is nullptr despite the shader having a AORoughnessMetallic map.")
						m_ORMMap->Bind(m_Shader->GetUniformValueInt(name));
					}

					else if (!m_UseCombinedORM && name.find("AmbientOcclusion") != std::string::npos)
					{
						LY_CORE_ASSERT(m_AmbientOcclusionMap, "AmbientOcclusionMap map is nullptr despite the shader having an AmbientOcclusion map.")
						m_AmbientOcclusionMap->Bind(m_Shader->GetUniformValueInt(name));
					}

					else if (!m_UseCombinedORM && name.find("Roughness") != std::string::npos)
					{
						LY_CORE_ASSERT(m_RoughnessMap, "RoughnessMapmap is nullptr despite the shader having a Roughness map.")
						m_RoughnessMap->Bind(m_Shader->GetUniformValueInt(name));
					}

					else if (!m_UseCombinedORM && name.find("Metallic") != std::string::npos)
					{
						LY_CORE_ASSERT(m_MetallicMap, "MetallicMap is nullptr despite the shader having a Metallic map.")
						m_MetallicMap->Bind(m_Shader->GetUniformValueInt(name));
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
