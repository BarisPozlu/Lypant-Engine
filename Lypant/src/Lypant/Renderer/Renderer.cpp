#include "lypch.h"
#include "Renderer.h"
#include <Lypant/Util/MeshFactory.h>
#include <Lypant/Util/Util.h>
#include "EditorPerspectiveCamera.h" // temp
#include <glm/glm.hpp> // temp
#include "Buffer.h" // temp

namespace lypant
{
	Renderer::RendererData* Renderer::s_Data = nullptr;
	RenderCommandBuffer* Renderer::s_Cmd = nullptr;

	void Renderer::Init()
	{
		util::MeshFactory::Create();

		s_Data = new RendererData();

		s_Cmd = RenderCommandBuffer::Create();

		std::shared_ptr<RenderTarget> defaultRenderTarget = RenderTarget::GetDefault();
		s_Data->QuadShader = Shader::Create("shaders/VulkanTest.glsl");
		//std::shared_ptr<Image> quadImage = Image2D::Create("textures/vulkan-test.png", ImageParams());
		ImageParams params;
		params.FloatingImage = true;
		std::shared_ptr<Image> quadImage = Image2D::Create("textures/skybox/example2.jpeg", params);
		float testArray[]
		{
			1.0f, 0.5f, 0.5f
		};
		//s_Data->QuadPass = Subpass::Create(defaultRenderTarget, s_Data->QuadShader, { { quadImage, 0 } }, 12, testArray, false);
		s_Data->QuadPass = Subpass::Create(defaultRenderTarget, false, s_Data->QuadShader, { { quadImage, 0 } }, 0, nullptr);

		s_Data->Camera = std::make_unique<EditorPerspectiveCamera>(glm::vec3(0.0f, 0.0f, 0.0f), glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
		s_Data->EnvironmentBuffer = StorageBuffer::Create(sizeof(glm::mat4) + 64, nullptr, true);
		s_Cmd->BindEnvironmentBuffer(s_Data->EnvironmentBuffer);
		
		// NOTE: Update this shader to vulkan
		s_Data->CubemapShader = Shader::Create("shaders/Skybox.glsl");
		std::shared_ptr<Image> cubemap = util::CreateCubemapFromEquirectangularImage("textures/skybox/example1.hdr");
		s_Data->CubemapPass = Subpass::Create(defaultRenderTarget, false, s_Data->CubemapShader, { { cubemap, 0 } }, 0, nullptr);
	}

	void Renderer::Shutdown()
	{
		delete s_Cmd;
		delete s_Data;
		util::MeshFactory::Destroy();
	}

	void Renderer::BeginRendering()
	{
		s_Cmd->BeginCommands();		
	}

	void Renderer::EndRendering()
	{
		s_Cmd->EndSubpass(*s_Data->QuadPass);
		s_Cmd->EndCommands();
	}

	void Renderer::BeginScene(float deltaTime)
	{
		s_Data->Camera->Tick(deltaTime);
		s_Data->EnvironmentBuffer->UploadData(&s_Data->Camera->GetViewProjectionMatrix(), sizeof(glm::mat4), 0);
		s_Data->EnvironmentBuffer->UploadData(&s_Data->Camera->GetPosition(), sizeof(glm::vec3), sizeof(glm::mat4));

		s_Cmd->BeginSubpass(*s_Data->CubemapPass);

		s_Cmd->DrawMesh(*util::MeshFactory::GetCubemapCube(), s_Data->CubemapShader);

		s_Cmd->EndSubpass(*s_Data->CubemapPass);

		s_Cmd->BeginSubpass(*s_Data->QuadPass);

		//s_Cmd->DrawMesh(*util::MeshFactory::GetQuad(), s_Data->QuadShader);


	}

	void Renderer::EndScene()
	{

	}

	void Renderer::SubmitMesh(const Mesh& mesh, const std::shared_ptr<Shader>& shader)
	{
		s_Cmd->DrawMesh(mesh, shader);
	}

	RenderCommandBuffer& Renderer::GetRenderCommandBuffer()
	{
		return *s_Cmd;
	}
}
