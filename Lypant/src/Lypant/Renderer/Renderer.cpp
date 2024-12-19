#include "lypch.h"
#include "Renderer.h"

namespace lypant
{
	Renderer::SceneData Renderer::s_SceneData;

	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(const std::shared_ptr<OrthographicCamera>& camera)
	{
		s_SceneData.Camera = camera;
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader)
	{
		vertexArray->Bind();
		shader->Bind();
		shader->SetUniformMatrix4Float("u_VP", (float*)&s_SceneData.Camera->GetViewProjectionMatrix()[0][0]);
		RenderCommand::DrawIndexed(vertexArray);
	}
}
