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

	void Renderer::BeginScene(const std::shared_ptr<PerspectiveCamera>& camera)
	{
		s_SceneData.Camera = camera;
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, const glm::mat4& modelMatrix)
	{
		vertexArray->Bind();
		shader->Bind();

		glm::mat4& MVP = s_SceneData.Camera->GetViewProjectionMatrix() * modelMatrix;
		shader->SetUniformMatrix4Float("u_MVP", &MVP[0][0]);
		shader->SetUniformMatrix4Float("u_ModelMatrix", (float*) &modelMatrix[0][0]); // temp
		shader->SetUniformMatrix3Float("u_NormalMatrix", (float*) &(glm::transpose(glm::inverse(glm::mat3(modelMatrix))))[0][0]); // temp
		
		RenderCommand::DrawIndexed(vertexArray);
	}
}
