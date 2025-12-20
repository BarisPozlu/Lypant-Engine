#pragma once

#include <memory>
#include "RenderCommandBuffer.h"
#include "RenderPass.h"
#include "Mesh.h"
#include "Lypant/Scene/Scene.h"

namespace lypant
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void BeginRendering();
		static void EndRendering();
		static void BeginScene(const Scene::SceneData& sceneData);
		static void EndScene();
		static void SubmitMesh(const Mesh& mesh, const glm::mat4& modelMatrix);
		static RenderCommandBuffer& GetRenderCommandBuffer();
	private:
		struct RendererData
		{
			std::unique_ptr<Subpass> LightingPass;
			std::unique_ptr<Subpass> CubemapPass;
			std::unique_ptr<Subpass> PostProcessPass;

			std::shared_ptr<class Buffer> EnvironmentBuffer;
		};
		static RendererData* s_Data;
		static RenderCommandBuffer* s_Cmd;
	};
}
