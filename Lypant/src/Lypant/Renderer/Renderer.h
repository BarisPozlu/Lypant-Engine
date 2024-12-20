#pragma once

#include "lypch.h"
#include "VertexArray.h"
#include "RenderCommand.h"
#include "Lypant/Camera/OrthographicCamera.h"
#include "Shader.h"

namespace lypant
{
	class Renderer
	{
	public:
		static void Init();
		static void OnWindowResize(uint32_t width, uint32_t height);
		static void BeginScene(const std::shared_ptr<OrthographicCamera>& camera);
		static void EndScene();

		static void Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader);
	private:
		struct SceneData
		{
			std::shared_ptr<OrthographicCamera> Camera;
		};

		static SceneData s_SceneData;
	};
}
