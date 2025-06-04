#include "lypch.h"
#include "Material.h"
#include "Lypant/Util/Textures.h"

namespace lypant
{
	Material::Material(const std::string& shaderPath, const std::string& albedoMapPath, const std::string& ormMapPath, const std::string& normalMapPath)
	{
		Shader = Shader::Load(shaderPath);
		AlbedoMap = Texture2D::Load(albedoMapPath, false);
		ORMMap = Texture2D::Load(ormMapPath);
		NormalMap = Texture2D::Load(normalMapPath);
		UseNormalMap = true;
		UseCombinedORM = true;
	}

	Material::Material(const std::string& shaderPath, const std::string& albedoMapPath, const std::string& roughnessMapPath, const std::string& metallicMapPath, const std::string& normalMapPath, const std::string& aoMapPath)
	{
		Shader = Shader::Load(shaderPath);
		AlbedoMap = Texture2D::Load(albedoMapPath, false);
		if (aoMapPath.size())
		{
			AmbientOcclusionMap = Texture2D::Load(aoMapPath);
		}
		RoughnessMap = Texture2D::Load(roughnessMapPath);
		MetallicMap = Texture2D::Load(metallicMapPath);
		NormalMap = Texture2D::Load(normalMapPath);
		UseNormalMap = true;
	}

	Material::Material(const std::string& shaderPath, const std::shared_ptr<Texture2D>& albedoMap, const std::shared_ptr<Texture2D>& ormMap, const std::shared_ptr<Texture2D>& normalMap)
	{
		Shader = Shader::Load(shaderPath);
		AlbedoMap = albedoMap;
		ORMMap = ormMap;
		NormalMap = normalMap;
		UseNormalMap = true;
		UseCombinedORM = true;
	}

	Material::Material(const std::string& shaderPath, const glm::vec3& albedo, float roughness, float metallic) : Roughness(roughness), Metallic(metallic)
	{
		Shader = Shader::Load(shaderPath);
		Albedo = albedo;
	}

	void Material::Bind() const
	{
		Shader->Bind();

		Shader->SetUniformInt("u_UseCombinedORM", UseCombinedORM);
		Shader->SetUniformInt("u_UseNormalMap", UseNormalMap);

		for (auto& [name, type] : Shader->GetUniformNamesToTypesMap())
		{
			switch (type)
			{
				case ShaderDataType::Float3:
					if (name.find("Albedo") != std::string::npos)
					{
						Shader->SetUniformVec3Float(name, reinterpret_cast<const float*>(&Albedo));
					}
					break;
				case ShaderDataType::Float:
					if (name.find("Roughness") != std::string::npos)
					{
						Shader->SetUniformFloat(name, Roughness);
					}

					else if (name.find("Metallic") != std::string::npos)
					{
						Shader->SetUniformFloat(name, Metallic);
					}
					break;
				case ShaderDataType::Sampler2D:
					if (name.find("Albedo") != std::string::npos)
					{
						int slot = Shader->GetUniformValueInt(name);

						if (AlbedoMap)
						{
							AlbedoMap->Bind(slot);
						}

						else
						{
							util::Textures::GetWhite3Channel1x1()->Bind(slot);
						}
					}

					else if (UseCombinedORM && name.find("ORM") != std::string::npos)
					{
						LY_CORE_ASSERT(ORMMap, "AORoughnessMetallic map is nullptr despite the shader having a AORoughnessMetallic map.")
						ORMMap->Bind(Shader->GetUniformValueInt(name));
					}

					else if (!UseCombinedORM && name.find("AmbientOcclusion") != std::string::npos)
					{
						LY_CORE_ASSERT(AmbientOcclusionMap, "AmbientOcclusionMap map is nullptr despite the shader having an AmbientOcclusion map.")
						AmbientOcclusionMap->Bind(Shader->GetUniformValueInt(name));
					}

					else if (!UseCombinedORM && name.find("Roughness") != std::string::npos)
					{
						int slot = Shader->GetUniformValueInt(name);

						if (RoughnessMap)
						{
							RoughnessMap->Bind(slot);
						}

						else
						{
							util::Textures::GetWhite1Channel1x1()->Bind(slot);
						}
					}

					else if (!UseCombinedORM && name.find("Metallic") != std::string::npos)
					{
						int slot = Shader->GetUniformValueInt(name);

						if (MetallicMap)
						{
							MetallicMap->Bind(slot);
						}

						else
						{
							util::Textures::GetWhite1Channel1x1()->Bind(slot);
						}
					}

					else if (name.find("Normal") != std::string::npos)
					{
						if (UseNormalMap)
						{
							LY_CORE_ASSERT(NormalMap, "Normal map is nullptr despite the shader having a Normal map.")
							NormalMap->Bind(Shader->GetUniformValueInt(name));
						}
					}
			}
		}
	}
}
