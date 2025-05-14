#include "lypch.h"
#include "Renderer.h"

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
				UpdateMSAAFrameBufferAttachments(2);
				break;
			case AntiAliasingSetting::MSAA4X:
				UpdateMSAAFrameBufferAttachments(4);
				break;
			case AntiAliasingSetting::MSAA8X:
				UpdateMSAAFrameBufferAttachments(8);
				break;
			case AntiAliasingSetting::MSAA16X:
				UpdateMSAAFrameBufferAttachments(16);
				break;
		}

		UpdatePostProcessFrameBufferAttachments();

		if (s_RendererData->IsBloomEnabled)
		{
			int currentWidth = width;
			int currentHeight = height;

			for (auto& texture : s_RendererData->BloomMipTextures)
			{
				currentWidth /= 2;
				currentHeight /= 2;

				texture.reset(new Texture2D(currentWidth, currentHeight, nullptr, true, true, 3, TextureWrappingOption::Clamp));
			}

			s_RendererData->BloomUpsampleShader->Bind();
			s_RendererData->BloomUpsampleShader->SetUniformFloat("u_AspectRatio", static_cast<float>(s_RendererData->WindowWidth) / s_RendererData->WindowHeight);
		}

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

		const std::shared_ptr<ColorAttachment>& sceneTexture = s_RendererData->PostProcessFrameBuffer->GetColorBuffer();

		if (s_RendererData->IsBloomEnabled)
		{
			CreateBloomTexture(reinterpret_cast<const std::shared_ptr<Texture2D>&>(sceneTexture));
			s_RendererData->BloomMipTextures[0]->Bind(s_RendererData->PostProcessShader->GetUniformValueInt("u_BloomTexture"));
		}

		sceneTexture->Bind(s_RendererData->PostProcessShader->GetUniformValueInt("u_SceneTexture"));

		RenderCommand::SetDepthTest(false);
		FrameBuffer::BindDefaultFrameBuffer();

		s_RendererData->PostProcessQuadVertexArray->Bind();
		s_RendererData->PostProcessShader->Bind();
		s_RendererData->PostProcessShader->SetUniformFloat("u_Exposure", s_RendererData->Exposure);
		s_RendererData->PostProcessShader->SetUniformInt("u_IsBloomEnabled", s_RendererData->IsBloomEnabled);
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
			s_RendererData->AntiAliasingSetting == AntiAliasingSetting::None ? CreateMSAAFrameBuffer(2) : UpdateMSAAFrameBufferAttachments(2);
			break;
		case AntiAliasingSetting::MSAA4X:
			s_RendererData->AntiAliasingSetting == AntiAliasingSetting::None ? CreateMSAAFrameBuffer(4) : UpdateMSAAFrameBufferAttachments(4);
			break;
		case AntiAliasingSetting::MSAA8X:
			s_RendererData->AntiAliasingSetting == AntiAliasingSetting::None ? CreateMSAAFrameBuffer(8) : UpdateMSAAFrameBufferAttachments(8);
			break;
		case AntiAliasingSetting::MSAA16X:
			s_RendererData->AntiAliasingSetting == AntiAliasingSetting::None ? CreateMSAAFrameBuffer(16) : UpdateMSAAFrameBufferAttachments(16);
			break;
		}

		s_RendererData->AntiAliasingSetting = setting;
	}

	void Renderer::SetExposure(float exposure)
	{
		s_RendererData->Exposure = exposure;
	}

	void Renderer::SetBloom(bool enabled)
	{
		if (s_RendererData->IsBloomEnabled == enabled) return;

		if (enabled)
		{
			CreateBloomResources();
		}

		else
		{
			DeleteBloomResources();
		}

		s_RendererData->IsBloomEnabled = enabled;
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
		UpdateMSAAFrameBufferAttachments(samples);
	}

	void Renderer::UpdateMSAAFrameBufferAttachments(uint32_t samples)
	{
		std::shared_ptr<Texture2DMultiSample> texture = std::make_shared<Texture2DMultiSample>(s_RendererData->WindowWidth, s_RendererData->WindowHeight, samples, true);
		std::shared_ptr<RenderBufferMultiSample> renderBuffer = std::make_shared<RenderBufferMultiSample>(s_RendererData->WindowWidth, s_RendererData->WindowHeight, samples);
		s_RendererData->MSAAFrameBuffer->AttachColorBuffer(texture);
		s_RendererData->MSAAFrameBuffer->AttachDepthStencilBuffer(renderBuffer);
	}

	void Renderer::CreatePostProcessFrameBuffer()
	{
		s_RendererData->PostProcessFrameBuffer = new FrameBuffer();
		UpdatePostProcessFrameBufferAttachments();
	}

	void Renderer::UpdatePostProcessFrameBufferAttachments()
	{
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

	void Renderer::CreateBloomResources()
	{
		s_RendererData->BloomFrameBuffer = new FrameBuffer();

		int currentWidth = s_RendererData->WindowWidth;
		int currentHeight = s_RendererData->WindowHeight;

		for (int i = 0; i < s_RendererData->BloomMipTextures.size(); i++)
		{
			// handle when the window size is too low
			currentWidth /= 2;
			currentHeight /= 2;

			s_RendererData->BloomMipTextures[i] = std::make_shared<Texture2D>(currentWidth, currentHeight, nullptr, true, true, 3, TextureWrappingOption::Clamp);
		}

		s_RendererData->BloomDownsampleShader = Shader::Load("shaders/BloomDownsample.glsl");
		s_RendererData->BloomUpsampleShader = Shader::Load("shaders/BloomUpsample.glsl");
		constexpr float filterRadius = 0.004f;
		s_RendererData->BloomUpsampleShader->Bind();
		s_RendererData->BloomUpsampleShader->SetUniformFloat("u_FilterRadius", filterRadius);
		s_RendererData->BloomUpsampleShader->SetUniformFloat("u_AspectRatio", static_cast<float>(s_RendererData->WindowWidth) / s_RendererData->WindowHeight);
	}

	void Renderer::DeleteBloomResources()
	{
		delete s_RendererData->BloomFrameBuffer;
		s_RendererData->BloomFrameBuffer = nullptr;
		s_RendererData->BloomDownsampleShader.reset();
		for (auto& texture : s_RendererData->BloomMipTextures)
		{
			texture.reset();
		}
	}

	void Renderer::CreateBloomTexture(const std::shared_ptr<Texture2D>& sceneTexture)
	{
		s_RendererData->PostProcessQuadVertexArray->Bind();

		// downsample
		s_RendererData->BloomDownsampleShader->Bind();

		glm::vec2 texelSize = 1.0f / sceneTexture->GetDimensions(); // texel size in texture coordinates
		s_RendererData->BloomDownsampleShader->SetUniformVec2Float("u_TexelSize", reinterpret_cast<float*>(&texelSize));

		sceneTexture->Bind(0);

		for (int i = 0; i < s_RendererData->BloomMipTextures.size(); i++)
		{
			auto& currentTexture = s_RendererData->BloomMipTextures[i];

			s_RendererData->BloomFrameBuffer->AttachColorBuffer(currentTexture);
			RenderCommand::SetViewport(0, 0, currentTexture->GetWidth(), currentTexture->GetHeight());
			RenderCommand::DrawIndexed(s_RendererData->PostProcessQuadVertexArray);

			texelSize = 1.0f / currentTexture->GetDimensions(); // texel size in texture coordinates
			s_RendererData->BloomDownsampleShader->SetUniformVec2Float("u_TexelSize", reinterpret_cast<float*>(&texelSize));

			currentTexture->Bind(0);
		}

		//upsample
		s_RendererData->BloomUpsampleShader->Bind();

		RenderCommand::SetBlending(true);
		RenderCommand::SetBlendFunction(BlendFunc::One_One);

		for (int i = s_RendererData->BloomMipTextures.size() - 1; i > 0; i--)
		{
			auto& currentTexture = s_RendererData->BloomMipTextures[i];
			auto& nextTexture = s_RendererData->BloomMipTextures[i - 1];

			currentTexture->Bind(0);

			s_RendererData->BloomFrameBuffer->AttachColorBuffer(nextTexture);
			RenderCommand::SetViewport(0, 0, nextTexture->GetWidth(), nextTexture->GetHeight());
			RenderCommand::DrawIndexed(s_RendererData->PostProcessQuadVertexArray);
		}

		RenderCommand::SetBlending(false);
		RenderCommand::SetBlendFunction(BlendFunc::SourceAlpha_OneMinusSourceAlpha);
		RenderCommand::SetViewport(0, 0, s_RendererData->WindowWidth, s_RendererData->WindowHeight);
		FrameBuffer::BindDefaultFrameBuffer();
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
		RenderCommand::SetViewport(0, 0, s_RendererData->WindowWidth, s_RendererData->WindowHeight);
	}
}
