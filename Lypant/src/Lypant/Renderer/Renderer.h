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
		static void BeginScene();
		static void EndScene();
		static void SubmitMesh(const Mesh& mesh, const std::shared_ptr<Shader>& shader);
		// TODO: Remove
		static RenderCommandBuffer& GetRenderCommandBuffer();
	private:
		struct RendererData
		{
			std::unique_ptr<Subpass> TestPass;
			std::shared_ptr<Shader> TestShader;
			std::shared_ptr<Image> TestImage;
			
		};
		static RendererData* s_Data;
		static RenderCommandBuffer* s_Cmd;
	};
}
