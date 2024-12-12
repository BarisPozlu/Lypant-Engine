#include "lypch.h"
#include "Renderer.h"

namespace lypant
{
	Renderer::SceneData Renderer::s_SceneData;

	void Renderer::BeginScene(const std::shared_ptr<OrthographicCamera>& camera)
	{
		s_SceneData.s_Camera = camera;
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader)
	{
		vertexArray->Bind();
		shader->Bind();
		shader->SetMatrix4FloatUniform("u_VP", (float*)&s_SceneData.s_Camera->GetViewProjectionMatrix()[0][0]);
		RenderCommand::DrawIndexed(vertexArray);
	}
}
