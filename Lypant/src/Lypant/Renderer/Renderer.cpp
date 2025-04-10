#include "lypch.h"
#include "Renderer.h"

namespace lypant
{
	Renderer::SceneData Renderer::s_SceneData;
	uint32_t Renderer::s_WindowWidth;
	uint32_t Renderer::s_WindowHeight;

	AntiAliasingSetting Renderer::s_AntiAliasingSetting = AntiAliasingSetting::None;
	FrameBuffer* Renderer::s_MSAAFrameBuffer = nullptr;

	FrameBuffer* Renderer::s_PostProcessFrameBuffer = nullptr;
	std::shared_ptr<VertexArray> Renderer::s_PostProcessQuadVertexArray;
	std::shared_ptr<Shader> Renderer::s_PostProcessShader;

	// getting rid of the vpointer, because it is not needed in the gpu.
	static constexpr int s_PurePointLightSize = sizeof(PointLight) - sizeof(void*);
	static constexpr int s_PureSpotLightSize = sizeof(SpotLight) - sizeof(void*);
	static constexpr int s_PureDirectionalLightSize = sizeof(DirectionalLight) - sizeof(void*);
	static constexpr int s_BufferSize = s_PurePointLightSize * 10 + s_PureDirectionalLightSize * 10;

	void Renderer::Init(uint32_t windowWidth, uint32_t windowHeight)
	{
		RenderCommand::Init();
		RenderCommand::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		s_WindowWidth = windowWidth;
		s_WindowHeight = windowHeight;

		s_SceneData.EnvironmentBuffer = new char[s_BufferSize];
		s_SceneData.EnvironmentUniformBuffer = new UniformBuffer(s_BufferSize, nullptr);

		CreatePostProcessFrameBuffer();
		CreatePostProcessQuadVertexArray();
		s_PostProcessShader = Shader::Load("shaders/PostProcess.glsl");
	}

	void Renderer::Shutdown()
	{
		s_SceneData.Lights.swap(std::vector<std::shared_ptr<Light>>());
		s_SceneData.Camera.reset();
		delete s_SceneData.EnvironmentUniformBuffer;
		delete[] s_SceneData.EnvironmentBuffer;

		delete s_MSAAFrameBuffer;

		delete s_PostProcessFrameBuffer;
		s_PostProcessQuadVertexArray.reset();
		s_PostProcessShader.reset();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);

		s_WindowWidth = width;
		s_WindowHeight = height;

		switch (s_AntiAliasingSetting)
		{
			case AntiAliasingSetting::MSAA2X:
				delete s_MSAAFrameBuffer;
				CreateMSAAFrameBuffer(2);
				break;
			case AntiAliasingSetting::MSAA4X:
				delete s_MSAAFrameBuffer;
				CreateMSAAFrameBuffer(4);
				break;
			case AntiAliasingSetting::MSAA8X:
				delete s_MSAAFrameBuffer;
				CreateMSAAFrameBuffer(8);
				break;
			case AntiAliasingSetting::MSAA16X:
				delete s_MSAAFrameBuffer;
				CreateMSAAFrameBuffer(16);
				break;
		}

		delete s_PostProcessFrameBuffer;
		CreatePostProcessFrameBuffer();
	}

	void Renderer::CreateMSAAFrameBuffer(uint32_t samples)
	{
		s_MSAAFrameBuffer = new FrameBuffer();
		std::shared_ptr<Texture2DMultiSample> texture = std::make_shared<Texture2DMultiSample>(s_WindowWidth, s_WindowHeight, samples);
		std::shared_ptr<RenderBufferMultiSample> renderBuffer = std::make_shared<RenderBufferMultiSample>(s_WindowWidth, s_WindowHeight, samples);
		s_MSAAFrameBuffer->AttachColorBuffer(texture);
		s_MSAAFrameBuffer->AttachDepthStencilBuffer(renderBuffer);
	}

	void Renderer::CreatePostProcessFrameBuffer()
	{
		s_PostProcessFrameBuffer = new FrameBuffer();
		std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>(s_WindowWidth, s_WindowHeight);
		std::shared_ptr<RenderBuffer> renderBuffer = std::make_shared<RenderBuffer>(s_WindowWidth, s_WindowHeight);
		s_PostProcessFrameBuffer->AttachColorBuffer(texture);
		s_PostProcessFrameBuffer->AttachDepthStencilBuffer(renderBuffer);
	}

	void Renderer::CreatePostProcessQuadVertexArray()
	{
		s_PostProcessQuadVertexArray = std::make_shared<VertexArray>();

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

		s_PostProcessQuadVertexArray->AddVertexBuffer(vertexBuffer);

		unsigned int indexData[]
		{
			0, 1, 2,
			2, 3, 0
		};

		std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>(indexData, 6);
		s_PostProcessQuadVertexArray->SetIndexBuffer(indexBuffer);
	}

	void Renderer::BeginScene(const std::shared_ptr<PerspectiveCamera>& camera, const std::vector<std::shared_ptr<Light>>& lights)
	{
		RenderCommand::SetDepthTest(true);

		if (s_MSAAFrameBuffer)
		{
			s_MSAAFrameBuffer->Bind();
		}

		else
		{
			s_PostProcessFrameBuffer->Bind();
		}

		RenderCommand::Clear();

		s_SceneData.Camera = camera;
		s_SceneData.Lights.clear();
		s_SceneData.Lights.reserve(lights.size());

		int numberOfLights[3] = { 0, 0, 0 };

		int pointLightInsertIndex = 0;
		int spotLightInsertIndex = 0;

		for (const auto& light : lights)
		{
			if (dynamic_cast<PointLight*>(light.get()))
			{
				s_SceneData.Lights.emplace(s_SceneData.Lights.begin() + pointLightInsertIndex, light);
				pointLightInsertIndex++;
				spotLightInsertIndex++;
				numberOfLights[LightTypePoint]++;
			}

			else if (dynamic_cast<SpotLight*>(light.get()))
			{
				s_SceneData.Lights.emplace(s_SceneData.Lights.begin() + spotLightInsertIndex, light);
				spotLightInsertIndex++;
				numberOfLights[LightTypeSpot]++;
			}

			else 
			{
				s_SceneData.Lights.emplace_back(light);
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
			memcpy(&s_SceneData.EnvironmentBuffer[offset], ((char*)s_SceneData.Lights.at(i).get()) + sizeof(void*), s_PurePointLightSize);
			offset += s_PurePointLightSize;
		}

		offset += pointLightsPadding;
		int offsetForSpotLight = offset;

		for (; i < numberOfLights[LightTypePoint] + numberOfLights[LightTypeSpot]; i++)
		{
			memcpy(&s_SceneData.EnvironmentBuffer[offset], ((char*)s_SceneData.Lights.at(i).get()) + sizeof(void*), s_PureSpotLightSize);
			offset += s_PureSpotLightSize;
		}

		offset += spotLightsPadding;
		int offsetForDirectionalLight = offset;

		for (; i < s_SceneData.Lights.size(); i++)
		{
			memcpy(&s_SceneData.EnvironmentBuffer[offset], ((char*)s_SceneData.Lights.at(i).get()) + sizeof(void*), s_PureDirectionalLightSize);
			offset += s_PureDirectionalLightSize;
		}
		
		offset += directionalLightPadding;
		int offsetForNumberOfLights = offset;

		constexpr int totalNumberOfLightsSize = sizeof(int) * 3;
		constexpr int totalNumberOfLightsPadding = 256 - totalNumberOfLightsSize;

		memcpy(&s_SceneData.EnvironmentBuffer[offset], numberOfLights, totalNumberOfLightsSize);
		
		offset += totalNumberOfLightsSize + totalNumberOfLightsPadding;
		int offsetForCamera = offset;

		memcpy(&s_SceneData.EnvironmentBuffer[offset], &s_SceneData.Camera->GetViewProjectionMatrix(), sizeof(glm::mat4));

		offset += sizeof(glm::mat4);

		memcpy(&s_SceneData.EnvironmentBuffer[offset], &s_SceneData.Camera->GetPosition(), sizeof(glm::vec3));

		offset += sizeof(glm::vec3);

		s_SceneData.EnvironmentUniformBuffer->BufferSubData(0, offset, s_SceneData.EnvironmentBuffer);

		if (numberOfLights[LightTypePoint] != 0)
		{
			s_SceneData.EnvironmentUniformBuffer->BindRange(0, 0, totalPointLightSize);
		}

		if (numberOfLights[LightTypeSpot] != 0)
		{
			s_SceneData.EnvironmentUniformBuffer->BindRange(1, offsetForSpotLight, totalSpotLightSize);
		}

		if (numberOfLights[LightTypeDirectional] != 0)
		{
			s_SceneData.EnvironmentUniformBuffer->BindRange(2, offsetForDirectionalLight, totalDirectionalLightSize);
		}

		s_SceneData.EnvironmentUniformBuffer->BindRange(3, offsetForNumberOfLights, totalNumberOfLightsSize);
		s_SceneData.EnvironmentUniformBuffer->BindRange(4, offsetForCamera, sizeof(glm::mat4) + sizeof(glm::vec3));
	}

	void Renderer::EndScene()
	{
		if (s_MSAAFrameBuffer)
		{
			s_MSAAFrameBuffer->BlitToFrameBuffer(s_PostProcessFrameBuffer, 0, 0, s_MSAAFrameBuffer->GetColorBufferWidth(), s_MSAAFrameBuffer->GetColorBufferHeight(), 0, 0, s_MSAAFrameBuffer->GetColorBufferWidth(), s_MSAAFrameBuffer->GetColorBufferHeight());
		}

		RenderCommand::SetDepthTest(false);
		FrameBuffer::BindDefaultFrameBuffer();
		s_PostProcessFrameBuffer->GetColorBuffer()->Bind(0);
		s_PostProcessQuadVertexArray->Bind();
		s_PostProcessShader->Bind();
		RenderCommand::DrawIndexed(s_PostProcessQuadVertexArray);
	}

	void Renderer::Submit(const std::shared_ptr<Mesh>& mesh, const glm::mat4& modelMatrix)
	{
		mesh->GetVertexArray()->Bind();
		mesh->GetMaterial()->Bind();

		// This is just a work around for now because we do not have a transform system.
		mesh->GetMaterial()->GetShader()->SetUniformMatrix4Float("u_ModelMatrix", (float*)&modelMatrix[0][0]);
		mesh->GetMaterial()->GetShader()->SetUniformMatrix3Float("u_NormalMatrix", (float*) &(glm::transpose(glm::inverse(glm::mat3(modelMatrix))))[0][0]);
		
		RenderCommand::DrawIndexed(mesh->GetVertexArray());
	}

	void Renderer::Submit(const std::shared_ptr<Model>& model, const glm::mat4& modelMatrix)
	{
		for (const Mesh& mesh : model->GetMeshes())
		{
			mesh.GetVertexArray()->Bind();
			mesh.GetMaterial()->Bind();

			// This is just a work around for now because we do not have a transform system.
			mesh.GetMaterial()->GetShader()->SetUniformMatrix4Float("u_ModelMatrix", (float*)&modelMatrix[0][0]);
			mesh.GetMaterial()->GetShader()->SetUniformMatrix3Float("u_NormalMatrix", (float*)&(glm::transpose(glm::inverse(glm::mat3(modelMatrix))))[0][0]);

			RenderCommand::DrawIndexed(mesh.GetVertexArray());
		}
	}

	void Renderer::Submit(const std::shared_ptr<Skybox>& skybox)
	{
		skybox->GetVertexArray()->Bind();
		skybox->GetShader()->Bind();
		skybox->GetCubemap()->Bind();

		RenderCommand::DrawIndexed(skybox->GetVertexArray());
	}

	void Renderer::SetAntiAliasing(AntiAliasingSetting setting)
	{
		if (setting == s_AntiAliasingSetting) return;

		switch (setting)
		{
			case AntiAliasingSetting::None:
				delete s_MSAAFrameBuffer;
				s_MSAAFrameBuffer = nullptr;
				FrameBuffer::BindDefaultFrameBuffer();
				break;
			case AntiAliasingSetting::MSAA2X:
				delete s_MSAAFrameBuffer;
				CreateMSAAFrameBuffer(2);
				break;
			case AntiAliasingSetting::MSAA4X:
				delete s_MSAAFrameBuffer;
				CreateMSAAFrameBuffer(4);
				break;
			case AntiAliasingSetting::MSAA8X:
				delete s_MSAAFrameBuffer;
				CreateMSAAFrameBuffer(8);
				break;
			case AntiAliasingSetting::MSAA16X:
				delete s_MSAAFrameBuffer;
				CreateMSAAFrameBuffer(16);
				break;
		}

		s_AntiAliasingSetting = setting;
	}
}
