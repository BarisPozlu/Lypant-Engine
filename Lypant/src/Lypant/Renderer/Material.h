#pragma once

#include "Shader.h"
#include "Texture.h"
#include <glm/glm.hpp>

// when pbr is implemented material system is going to change, but for now it should do the job.
// also some stuff may not be needed in this implementation but it is good to keep it for the future.

namespace lypant
{
	class Material
	{
	public:
		Material(const std::string& shaderPath, const std::string& diffuseMapPath, const std::string& specularMapPath, const std::string& normalMapPath = std::string());
		Material(const std::string& shaderPath, const glm::vec3& color);
		~Material();
		void Bind() const;
		void UpdateColor(const glm::vec3& color);

		// This is just a work around for now because we do not have a transform system.
		inline const std::shared_ptr<Shader>& GetShader() const { return m_Shader; }
	private:
		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<Texture2D> m_DiffuseMap;
		std::shared_ptr<Texture2D> m_SpecularMap;
		std::shared_ptr<Texture2D> m_NormalMap;
		char* m_Buffer; // uniforms and textures
	};
}
