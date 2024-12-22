#pragma once

#include "lypch.h"
#include "VertexArray.h"
#include "RenderCommand.h"
#include "Lypant/Camera/PerspectiveCamera.h"
#include "Shader.h"
#include "glm/glm.hpp"

namespace lypant
{
	class Renderer
	{
	public:
		static void Init();
		static void OnWindowResize(uint32_t width, uint32_t height);
		static void BeginScene(const std::shared_ptr<PerspectiveCamera>& camera);
		static void EndScene();

		static void Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, const glm::mat4& modelMatrix);
	private:
		struct SceneData
		{
			std::shared_ptr<PerspectiveCamera> Camera;
		};

		static SceneData s_SceneData;
	};
}
