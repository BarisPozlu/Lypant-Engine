#pragma once

#include "Shader.h"
#include "Texture.h"
#include <glm/glm.hpp>

namespace lypant
{
	class Material
	{
	public:
		Material(const std::string& shaderPath, const std::string& albedoMapPath, const std::string& aoRoughnessMetallicMapPath, const std::string& normalMapPath);
		Material(const std::string& shaderPath, const std::shared_ptr<Texture2D>& albedoMap, const std::shared_ptr<Texture2D>& aoRoughnessMetallicMap, const std::shared_ptr<Texture2D>& normalMap);
		Material(const std::string& shaderPath, const glm::vec3& color);
		~Material();
		void Bind() const;
		void UpdateColor(const glm::vec3& color);

		// This is just a work around for now because we do not have a transform system.
		inline const std::shared_ptr<Shader>& GetShader() const { return m_Shader; }
	private:
		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<Texture2D> m_AlbedoMap;
		std::shared_ptr<Texture2D> m_AORoughnessMetallicMap;
		std::shared_ptr<Texture2D> m_NormalMap;
		char* m_Buffer;
	};
}
