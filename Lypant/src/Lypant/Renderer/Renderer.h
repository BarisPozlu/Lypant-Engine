#pragma once

#include <memory>
#include "RenderCommandBuffer.h"
#include "RenderPass.h"
#include "Mesh.h"

namespace lypant
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void BeginRendering();
		static void EndRendering();
		// TODO: delta time is there for testing, remove
		static void BeginScene(float deltaTime);
		static void EndScene();
		static void SubmitMesh(const Mesh& mesh, const std::shared_ptr<Shader>& shader);
		static RenderCommandBuffer& GetRenderCommandBuffer();
	private:
		struct RendererData
		{
			std::unique_ptr<Subpass> QuadPass;
			std::shared_ptr<Shader> QuadShader;
			std::unique_ptr<Subpass> CubemapPass;
			std::shared_ptr<Shader> CubemapShader;

			std::unique_ptr<class EditorPerspectiveCamera> Camera;
			std::shared_ptr<class Buffer> EnvironmentBuffer;
		};
		static RendererData* s_Data;
		static RenderCommandBuffer* s_Cmd;
	};
}
