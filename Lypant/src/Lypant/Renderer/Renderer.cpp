#include "lypch.h"
#include "Renderer.h"
#include <Lypant/Util/MeshFactory.h>
#include <Lypant/Util/Util.h>
#include <Lypant/Util/Images.h>
#include "EditorPerspectiveCamera.h" // temp
#include <glm/glm.hpp> // temp
#include "Buffer.h" // temp
#include "EnvironmentBufferLayout.h"

namespace lypant
{
	Renderer::RendererData* Renderer::s_Data = nullptr;
	RenderCommandBuffer* Renderer::s_Cmd = nullptr;

	void Renderer::Init()
	{
		s_Data = new RendererData();
		s_Cmd = RenderCommandBuffer::Create();

		util::MeshFactory::Create();
		util::Images::Create();

		s_Data->EnvironmentBuffer = Buffer::CreateStorageBuffer(sizeof(EnvironmentBufferLayout), nullptr, true);
		s_Cmd->BindEnvironmentBuffer(s_Data->EnvironmentBuffer);

		// Create Lighting pass

		std::shared_ptr<Image> cubemap = util::CreateCubemapFromEquirectangularImage("textures/skybox/example1.hdr");
		std::shared_ptr<Image> diffuseIrradiance = util::CreateDiffuseIrradianceMap(cubemap);
		std::shared_ptr<Image> prefiltered = util::CreatePreFilteredMap(cubemap);
		std::shared_ptr<Image> brdf = util::Images::GetBRDFIntegrationMap();

		ImageParams params;
		params.FloatingImage = true;

		std::shared_ptr<Image> colorBuffer = Image::CreateImage2DRenderTarget(1280, 720, 4, params, true);
		std::shared_ptr<Image> depthBuffer = Image::CreateDepthImage2D(1280, 720, { }, false);

 		std::shared_ptr<RenderTarget> hdrTarget = RenderTarget::Create();
		hdrTarget->AttachColorBuffer(colorBuffer);
		hdrTarget->AttachDepthStencilBuffer(depthBuffer);

		RenderTargetOperation op;
		op.ColorBufferLoadOp = AttachmentLoadOperation::Clear;
		op.ColorBufferStoreOp = AttachmentStoreOperation::Store;
		op.DepthBufferLoadOp = AttachmentLoadOperation::Clear;
		op.DepthBufferStoreOp = AttachmentStoreOperation::Store;

		// TODO: the way we give the size is gonna change
		s_Data->LightingPass = Subpass::Create(hdrTarget, op, Shader::Create("shaders/Model_PBR.glsl"), 
			{ { diffuseIrradiance, 0 }, { prefiltered, 1 }, { brdf, 2 } }, 192, nullptr, true, SubpassFlagDrawWithMaterial);

		// Create Cubemap pass

		op.ColorBufferLoadOp = AttachmentLoadOperation::Load;
		op.DepthBufferLoadOp = AttachmentLoadOperation::Load;
		op.DepthBufferStoreOp = AttachmentStoreOperation::DontCare;

		s_Data->CubemapPass = Subpass::Create(hdrTarget, op, Shader::Create("shaders/Skybox.glsl"), { { cubemap, 0 } }, 0, nullptr);
		s_Data->CubemapPass->Submit(*util::MeshFactory::GetCubemapCube(), glm::mat4(1.0f));

		// Create Post-process pass

		op.ColorBufferLoadOp = AttachmentLoadOperation::DontCare;

		s_Data->PostProcessPass = Subpass::Create(RenderTarget::GetDefault(), op, Shader::Create("shaders/PostProcess.glsl"), { { colorBuffer, 0 } }, 0, nullptr);
		s_Data->PostProcessPass->Submit(*util::MeshFactory::GetQuad(), glm::mat4(1.0f));
	}

	void Renderer::Shutdown()
	{
		util::Images::Destroy();
		util::MeshFactory::Destroy();
		delete s_Cmd;
		delete s_Data;
	}

	bool Renderer::BeginRendering()
	{
		// TODO: Update
		s_Data->PostProcessPass->SetRenderTarget(RenderTarget::GetDefault());
		return s_Cmd->BeginCommands();
	}

	void Renderer::EndRendering()
	{
		s_Cmd->EndSubpass(*s_Data->PostProcessPass);
		s_Cmd->EndCommands();
	}

	void Renderer::BeginScene(const Scene::SceneData& sceneData)
	{
		s_Data->EnvironmentBuffer->UploadData(&sceneData.Camera->GetViewProjectionMatrix(), sizeof(glm::mat4), 0);
		s_Data->EnvironmentBuffer->UploadData(&sceneData.Camera->GetViewMatrix(), sizeof(glm::mat4), sizeof(glm::mat4));
		s_Data->EnvironmentBuffer->UploadData(&sceneData.Camera->GetPosition(), sizeof(glm::vec3), 2 * sizeof(glm::mat4));
		

		s_Data->EnvironmentBuffer->UploadData(&sceneData.NumberOfDirectionalLights, sizeof(sceneData.NumberOfDirectionalLights), EnvironmentBufferLayout::GetOffsetOfBinding(1) + offsetof(LightCountData, DirectionalLightCount));
		s_Data->EnvironmentBuffer->UploadData(sceneData.DirectionalLightComponents, sceneData.NumberOfDirectionalLights * sizeof(DirectionalLightComponent), EnvironmentBufferLayout::GetOffsetOfBinding(4));

		s_Data->LightingPass->ClearDrawData();
	}

	void Renderer::EndScene()
	{
		s_Cmd->ExecuteSubpass(*s_Data->LightingPass);
		s_Cmd->ExecuteSubpass(*s_Data->CubemapPass);
		s_Cmd->ExecuteSubpass(*s_Data->PostProcessPass);

		// for now to draw ImGui I will begin the post process pass again and end it after the Imgui also has rendered
		s_Cmd->BeginSubpass(*s_Data->PostProcessPass);
	}

	void Renderer::SubmitMesh(const Mesh& mesh, const glm::mat4& modelMatrix)
	{
		s_Data->LightingPass->Submit(mesh, modelMatrix, 1);
	}

	RenderCommandBuffer& Renderer::GetRenderCommandBuffer()
	{
		return *s_Cmd;
	}
}
