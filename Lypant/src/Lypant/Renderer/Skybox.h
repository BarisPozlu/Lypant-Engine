#pragma once

#include "lypch.h"

namespace lypant
{
	class VertexArray;
	class Shader;
	class Cubemap;

	class Skybox
	{
	public:
		Skybox(const std::string& path);

		const std::shared_ptr<VertexArray>& GetVertexArray() const { return m_VertexArray; }
		const std::shared_ptr<Shader>& GetShader() const { return m_Shader; }
		const std::unique_ptr<Cubemap>& GetCubemap() const { return m_Cubemap; }
	private:
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<Shader> m_Shader;
		std::unique_ptr<Cubemap> m_Cubemap;
	};
}
