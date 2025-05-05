#include "lypch.h"
#include "Renderer.h"
#include "Lypant/Core/Application.h"

namespace lypant
{
	Renderer::RendererData* Renderer::s_RendererData = nullptr;

	// getting rid of the vpointer, because it is not needed in the gpu.
	static constexpr int s_PurePointLightSize = sizeof(PointLight) - sizeof(void*);
	static constexpr int s_PureSpotLightSize = sizeof(SpotLight) - sizeof(void*);
	static constexpr int s_PureDirectionalLightSize = sizeof(DirectionalLight) - sizeof(void*);
	static constexpr int s_BufferSize = s_PurePointLightSize * 10 + s_PureDirectionalLightSize * 10;

	void Renderer::Init(uint32_t windowWidth, uint32_t windowHeight)
	{
		RenderCommand::Init();
		RenderCommand::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		s_RendererData = new RendererData();

		s_RendererData->WindowWidth = windowWidth;
		s_RendererData->WindowHeight = windowHeight;

		s_RendererData->EnvironmentBuffer = new char[s_BufferSize];
		s_RendererData->EnvironmentUniformBuffer = new UniformBuffer(s_BufferSize, nullptr);

		CreatePostProcessFrameBuffer();
		CreatePostProcessQuadVertexArray();
		s_RendererData->PostProcessShader = Shader::Load("shaders/PostProcess.glsl");

		CreateBRDFIntegrationMap();
	}

	void Renderer::Shutdown()
	{
		delete s_RendererData;
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);

		s_RendererData->WindowWidth = width;
		s_RendererData->WindowHeight = height;

		switch (s_RendererData->AntiAliasingSetting)
		{
			case AntiAliasingSetting::MSAA2X:
				delete s_RendererData->MSAAFrameBuffer;
				CreateMSAAFrameBuffer(2);
				break;
			case AntiAliasingSetting::MSAA4X:
				delete s_RendererData->MSAAFrameBuffer;
				CreateMSAAFrameBuffer(4);
				break;
			case AntiAliasingSetting::MSAA8X:
				delete s_RendererData->MSAAFrameBuffer;
				CreateMSAAFrameBuffer(8);
				break;
			case AntiAliasingSetting::MSAA16X:
				delete s_RendererData->MSAAFrameBuffer;
				CreateMSAAFrameBuffer(16);
				break;
		}

		delete s_RendererData->PostProcessFrameBuffer;
		CreatePostProcessFrameBuffer();
	}

	void Renderer::BeginScene(const std::shared_ptr<PerspectiveCamera>& camera, const std::vector<std::shared_ptr<Light>>& lights, const std::shared_ptr<Skybox>& environmentMap)
	{
		if (s_RendererData->EnvironmentMap != environmentMap)
		{
			s_RendererData->DiffuseIrradianceMap = environmentMap->GetCubemap()->GetDiffuseIrradianceMap();
			s_RendererData->PrefilteredMap = environmentMap->GetCubemap()->GetPreFilteredMap();
		}

		RenderCommand::SetDepthTest(true);

		if (s_RendererData->MSAAFrameBuffer)
		{
			s_RendererData->MSAAFrameBuffer->Bind();
		}

		else
		{
			s_RendererData->PostProcessFrameBuffer->Bind();
		}

		RenderCommand::Clear();

		s_RendererData->Camera = camera;
		s_RendererData->EnvironmentMap = environmentMap;
		s_RendererData->Lights.clear();
		s_RendererData->Lights.reserve(lights.size());

		UpdateEnvironmentBuffers(lights);
	}

	void Renderer::EndScene()
	{
		Submit(s_RendererData->EnvironmentMap);

		if (s_RendererData->MSAAFrameBuffer)
		{
			s_RendererData->MSAAFrameBuffer->BlitToFrameBuffer(s_RendererData->PostProcessFrameBuffer, 0, 0, s_RendererData->MSAAFrameBuffer->GetColorBufferWidth(), s_RendererData->MSAAFrameBuffer->GetColorBufferHeight(), 0, 0, s_RendererData->MSAAFrameBuffer->GetColorBufferWidth(), s_RendererData->MSAAFrameBuffer->GetColorBufferHeight());
		}

		RenderCommand::SetDepthTest(false);
		FrameBuffer::BindDefaultFrameBuffer();
		s_RendererData->PostProcessFrameBuffer->GetColorBuffer()->Bind(0);
		s_RendererData->PostProcessQuadVertexArray->Bind();
		s_RendererData->PostProcessShader->Bind();
		s_RendererData->PostProcessShader->SetUniformFloat("u_Exposure", s_RendererData->Exposure);
		RenderCommand::DrawIndexed(s_RendererData->PostProcessQuadVertexArray);
	}

	void Renderer::Submit(const std::shared_ptr<Mesh>& mesh, const glm::mat4& modelMatrix)
	{
		s_RendererData->DiffuseIrradianceMap->Bind(0);
		s_RendererData->PrefilteredMap->Bind(1);
		s_RendererData->BRDFIntegrationMap->Bind(2);

		mesh->GetVertexArray()->Bind();
		mesh->GetMaterial()->Bind();

		// This is just a work around for now because we do not have a transform system.
		mesh->GetMaterial()->GetShader()->SetUniformMatrix4Float("u_ModelMatrix", (float*)&modelMatrix[0][0]);
		mesh->GetMaterial()->GetShader()->SetUniformMatrix3Float("u_NormalMatrix", (float*)&(glm::transpose(glm::inverse(glm::mat3(modelMatrix))))[0][0]);

		RenderCommand::DrawIndexed(mesh->GetVertexArray());
	}

	void Renderer::Submit(const std::shared_ptr<Model>& model, const glm::mat4& modelMatrix)
	{
		s_RendererData->DiffuseIrradianceMap->Bind(0);
		s_RendererData->PrefilteredMap->Bind(1);
		s_RendererData->BRDFIntegrationMap->Bind(2);

		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));

		for (const Mesh& mesh : model->GetMeshes())
		{
			mesh.GetVertexArray()->Bind();
			mesh.GetMaterial()->Bind();

			// This is just a work around for now because we do not have a transform system.
			mesh.GetMaterial()->GetShader()->SetUniformMatrix4Float("u_ModelMatrix", (float*)&modelMatrix[0][0]);
			mesh.GetMaterial()->GetShader()->SetUniformMatrix3Float("u_NormalMatrix", (float*)&normalMatrix[0][0]);

			RenderCommand::DrawIndexed(mesh.GetVertexArray());
		}
	}

	void Renderer::Submit(const std::shared_ptr<Skybox>& skybox)
	{
		skybox->GetVertexArray()->Bind();
		skybox->GetShader()->Bind();
		skybox->GetCubemap()->Bind(0);

		RenderCommand::DrawIndexed(skybox->GetVertexArray());
	}

	void Renderer::SetAntiAliasing(AntiAliasingSetting setting)
	{
		if (setting == s_RendererData->AntiAliasingSetting) return;

		switch (setting)
		{
		case AntiAliasingSetting::None:
			delete s_RendererData->MSAAFrameBuffer;
			s_RendererData->MSAAFrameBuffer = nullptr;
			FrameBuffer::BindDefaultFrameBuffer();
			break;
		case AntiAliasingSetting::MSAA2X:
			delete s_RendererData->MSAAFrameBuffer;
			CreateMSAAFrameBuffer(2);
			break;
		case AntiAliasingSetting::MSAA4X:
			delete s_RendererData->MSAAFrameBuffer;
			CreateMSAAFrameBuffer(4);
			break;
		case AntiAliasingSetting::MSAA8X:
			delete s_RendererData->MSAAFrameBuffer;
			CreateMSAAFrameBuffer(8);
			break;
		case AntiAliasingSetting::MSAA16X:
			delete s_RendererData->MSAAFrameBuffer;
			CreateMSAAFrameBuffer(16);
			break;
		}

		s_RendererData->AntiAliasingSetting = setting;
	}

	void Renderer::SetExposure(float exposure)
	{
		s_RendererData->Exposure = exposure;
	}

	void Renderer::UpdateEnvironmentBuffers(const std::vector<std::shared_ptr<Light>>& lights)
	{
		int numberOfLights[3] = { 0, 0, 0 };

		int pointLightInsertIndex = 0;
		int spotLightInsertIndex = 0;

		for (const auto& light : lights)
		{
			if (dynamic_cast<PointLight*>(light.get()))
			{
				s_RendererData->Lights.emplace(s_RendererData->Lights.begin() + pointLightInsertIndex, light);
				pointLightInsertIndex++;
				spotLightInsertIndex++;
				numberOfLights[LightTypePoint]++;
			}

			else if (dynamic_cast<SpotLight*>(light.get()))
			{
				s_RendererData->Lights.emplace(s_RendererData->Lights.begin() + spotLightInsertIndex, light);
				spotLightInsertIndex++;
				numberOfLights[LightTypeSpot]++;
			}

			else
			{
				s_RendererData->Lights.emplace_back(light);
				numberOfLights[LightTypeDirectional]++;
			}
		}

		int totalPointLightSize = s_PurePointLightSize * numberOfLights[LightTypePoint];
		int pointLightsPadding = 256 - (totalPointLightSize % 256);

		int totalSpotLightSize = s_PureSpotLightSize * numberOfLights[LightTypeSpot];
		int spotLightsPadding = 256 - (totalSpotLightSize % 256);

		int totalDirectionalLightSize = s_PureDirectionalLightSize * numberOfLights[LightTypeDirectional];
		int directionalLightPadding = 256 - (totalDirectionalLightSize % 256);

		int offset = 0;
		int i;

		for (i = 0; i < numberOfLights[LightTypePoint]; i++)
		{
			memcpy(&s_RendererData->EnvironmentBuffer[offset], ((char*)s_RendererData->Lights.at(i).get()) + sizeof(void*), s_PurePointLightSize);
			offset += s_PurePointLightSize;
		}

		offset += pointLightsPadding;
		int offsetForSpotLight = offset;

		for (; i < numberOfLights[LightTypePoint] + numberOfLights[LightTypeSpot]; i++)
		{
			memcpy(&s_RendererData->EnvironmentBuffer[offset], ((char*)s_RendererData->Lights.at(i).get()) + sizeof(void*), s_PureSpotLightSize);
			offset += s_PureSpotLightSize;
		}

		offset += spotLightsPadding;
		int offsetForDirectionalLight = offset;

		for (; i < s_RendererData->Lights.size(); i++)
		{
			memcpy(&s_RendererData->EnvironmentBuffer[offset], ((char*)s_RendererData->Lights.at(i).get()) + sizeof(void*), s_PureDirectionalLightSize);
			offset += s_PureDirectionalLightSize;
		}

		offset += directionalLightPadding;
		int offsetForNumberOfLights = offset;

		constexpr int totalNumberOfLightsSize = sizeof(int) * 3;
		constexpr int totalNumberOfLightsPadding = 256 - totalNumberOfLightsSize;

		memcpy(&s_RendererData->EnvironmentBuffer[offset], numberOfLights, totalNumberOfLightsSize);

		offset += totalNumberOfLightsSize + totalNumberOfLightsPadding;
		int offsetForCamera = offset;

		memcpy(&s_RendererData->EnvironmentBuffer[offset], &s_RendererData->Camera->GetViewProjectionMatrix(), sizeof(glm::mat4));

		offset += sizeof(glm::mat4);

		memcpy(&s_RendererData->EnvironmentBuffer[offset], &s_RendererData->Camera->GetPosition(), sizeof(glm::vec3));

		offset += sizeof(glm::vec3);

		s_RendererData->EnvironmentUniformBuffer->BufferSubData(0, offset, s_RendererData->EnvironmentBuffer);

		if (numberOfLights[LightTypePoint] != 0)
		{
			s_RendererData->EnvironmentUniformBuffer->BindRange(0, 0, totalPointLightSize);
		}

		if (numberOfLights[LightTypeSpot] != 0)
		{
			s_RendererData->EnvironmentUniformBuffer->BindRange(1, offsetForSpotLight, totalSpotLightSize);
		}

		if (numberOfLights[LightTypeDirectional] != 0)
		{
			s_RendererData->EnvironmentUniformBuffer->BindRange(2, offsetForDirectionalLight, totalDirectionalLightSize);
		}

		s_RendererData->EnvironmentUniformBuffer->BindRange(3, offsetForNumberOfLights, totalNumberOfLightsSize);
		s_RendererData->EnvironmentUniformBuffer->BindRange(4, offsetForCamera, sizeof(glm::mat4) + sizeof(glm::vec3));
	}

	void Renderer::CreateMSAAFrameBuffer(uint32_t samples)
	{
		s_RendererData->MSAAFrameBuffer = new FrameBuffer();
		std::shared_ptr<Texture2DMultiSample> texture = std::make_shared<Texture2DMultiSample>(s_RendererData->WindowWidth, s_RendererData->WindowHeight, samples, true);
		std::shared_ptr<RenderBufferMultiSample> renderBuffer = std::make_shared<RenderBufferMultiSample>(s_RendererData->WindowWidth, s_RendererData->WindowHeight, samples);
		s_RendererData->MSAAFrameBuffer->AttachColorBuffer(texture);
		s_RendererData->MSAAFrameBuffer->AttachDepthStencilBuffer(renderBuffer);
	}

	void Renderer::CreatePostProcessFrameBuffer()
	{
		s_RendererData->PostProcessFrameBuffer = new FrameBuffer();
		std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>(s_RendererData->WindowWidth, s_RendererData->WindowHeight, nullptr, true, true);
		std::shared_ptr<RenderBuffer> renderBuffer = std::make_shared<RenderBuffer>(s_RendererData->WindowWidth, s_RendererData->WindowHeight);
		s_RendererData->PostProcessFrameBuffer->AttachColorBuffer(texture);
		s_RendererData->PostProcessFrameBuffer->AttachDepthStencilBuffer(renderBuffer);
	}

	void Renderer::CreatePostProcessQuadVertexArray()
	{
		s_RendererData->PostProcessQuadVertexArray = std::make_shared<VertexArray>();

		float vertexData[]
		{
			-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f,  0.0f, 1.0f
		};

		std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>(vertexData, sizeof(vertexData));

		BufferLayout layout
		{
			{ShaderDataType::Float3, "a_Position"}, {ShaderDataType::Float2, "a_TexCoord"}
		};

		vertexBuffer->SetLayout(layout);

		s_RendererData->PostProcessQuadVertexArray->AddVertexBuffer(vertexBuffer);

		unsigned int indexData[]
		{
			0, 1, 2,
			2, 3, 0
		};

		std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>(indexData, 6);
		s_RendererData->PostProcessQuadVertexArray->SetIndexBuffer(indexBuffer);
	}

	void Renderer::CreateBRDFIntegrationMap()
	{
		s_RendererData->BRDFIntegrationMap = std::make_shared<Texture2D>(512, 512, nullptr, true, true, 2, TextureWrappingOption::Clamp);
		std::shared_ptr<Shader> shader = Shader::Load("shaders/CalculateBRDFIntegrationMap.glsl");

		RenderCommand::SetViewport(0, 0, 512, 512);

		FrameBuffer framebuffer;

		framebuffer.AttachColorBuffer(s_RendererData->BRDFIntegrationMap);

		s_RendererData->PostProcessQuadVertexArray->Bind();
		shader->Bind();
		RenderCommand::DrawIndexed(s_RendererData->PostProcessQuadVertexArray);

		FrameBuffer::BindDefaultFrameBuffer();
		RenderCommand::SetViewport(0, 0, Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());
	}
}
