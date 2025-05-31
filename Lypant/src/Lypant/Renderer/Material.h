#pragma once

#include "Shader.h"
#include "Texture.h"
#include <glm/glm.hpp>

// the current material system is bad, i'll rewrite it in the future

namespace lypant
{
	class Material
	{
	public:
		Material(const std::string& shaderPath, const std::string& albedoMapPath, const std::string& ormMapPath, const std::string& normalMapPath);
		Material(const std::string& shaderPath, const std::string& albedoMapPath, const std::string& roughnessMapPath, const std::string& metallicMapPath, const std::string& normalMapPath, const std::string& aoMapPath = std::string());
		Material(const std::string& shaderPath, const std::shared_ptr<Texture2D>& albedoMap, const std::shared_ptr<Texture2D>& ormMap, const std::shared_ptr<Texture2D>& normalMap);
		Material(const std::string& shaderPath, const glm::vec3& color);
		~Material();
		void Bind() const;
		void UpdateColor(const glm::vec3& color);

		inline const std::shared_ptr<Shader>& GetShader() const { return m_Shader; }
	private:
		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<Texture2D> m_AlbedoMap;
		std::shared_ptr<Texture2D> m_ORMMap;
		std::shared_ptr<Texture2D> m_AmbientOcclusionMap;
		std::shared_ptr<Texture2D> m_RoughnessMap;
		std::shared_ptr<Texture2D> m_MetallicMap;
		std::shared_ptr<Texture2D> m_NormalMap;
		bool m_UseCombinedORM;
		char* m_Buffer;
	};
}
