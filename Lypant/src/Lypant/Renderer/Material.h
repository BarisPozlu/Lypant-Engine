#pragma once

#include "Shader.h"
#include "Texture.h"
#include <glm/glm.hpp>

// the current material system is very simple and can be made better. But it does the job for now

namespace lypant
{
	class Material
	{
	public:
		Material() = default;
		Material(const std::string& shaderPath, const std::string& albedoMapPath, const std::string& ormMapPath, const std::string& normalMapPath);
		Material(const std::string& shaderPath, const std::string& albedoMapPath, const std::string& roughnessMapPath, const std::string& metallicMapPath, const std::string& normalMapPath, const std::string& aoMapPath = std::string());
		Material(const std::string& shaderPath, const std::shared_ptr<Texture2D>& albedoMap, const std::shared_ptr<Texture2D>& ormMap, const std::shared_ptr<Texture2D>& normalMap);
		Material(const std::string& shaderPath, const glm::vec3& albedo, float roughness = 0.5f, float metallic = 0.0f);
		~Material() = default;
		void Bind() const;
	public:
		std::shared_ptr<Shader> Shader;
		std::shared_ptr<Texture2D> AlbedoMap;
		std::shared_ptr<Texture2D> ORMMap;
		std::shared_ptr<Texture2D> AmbientOcclusionMap;
		std::shared_ptr<Texture2D> RoughnessMap;
		std::shared_ptr<Texture2D> MetallicMap;
		std::shared_ptr<Texture2D> NormalMap;
		glm::vec3 Albedo = glm::vec3(1.0f);
		float Roughness = 1.0f;
		float Metallic = 1.0f;
		bool UseCombinedORM = false;
		bool UseNormalMap = false;
	};
}
