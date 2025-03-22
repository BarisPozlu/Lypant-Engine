#pragma once

#include "lypch.h"
#include "VertexArray.h"
#include "RenderCommand.h"
#include "Lypant/Camera/PerspectiveCamera.h"
#include "Material.h"
#include "glm/glm.hpp"
#include "Lypant/Light/Light.h"
#include "Mesh.h"
#include "Model.h"
#include "Skybox.h"

namespace lypant
{
	enum class AntiAliasingSetting
	{
		None, MSAA2X, MSAA4X, MSAA8X, MSAA16X
	};

	class Renderer
	{
	public:
		friend class Application;

		static void BeginScene(const std::shared_ptr<PerspectiveCamera>& camera, const std::vector<std::shared_ptr<Light>>& lights);
		static void EndScene();
		static void Submit(const std::shared_ptr<Mesh>& mesh, const glm::mat4& modelMatrix);
		static void Submit(const std::shared_ptr<Model>& model, const glm::mat4& modelMatrix);
		static void Submit(const std::shared_ptr<Skybox>& skybox);
		// You should set anti aliasing before you call BeginScene().
		static void SetAntiAliasing(AntiAliasingSetting setting);
	private:
		static void Init(uint32_t windowWidth, uint32_t windowHeight);
		static void Shutdown();
		static void OnWindowResize(uint32_t width, uint32_t height);
		static void CreateMSAAFrameBuffer(uint32_t samples);
	private:
		struct SceneData
		{
			std::vector<std::shared_ptr<Light>> Lights;
			std::shared_ptr<PerspectiveCamera> Camera;

			std::unique_ptr<UniformBuffer> EnvironmentUniformBuffer;
			char* EnvironmentBuffer;
		};

		static SceneData s_SceneData;
		static uint32_t s_WindowWidth;
		static uint32_t s_WindowHeight;
		static AntiAliasingSetting s_AntiAliasingSetting;
		static FrameBuffer* s_MSAAFrameBuffer;
	};
}
