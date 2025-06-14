#include "lypch.h"
#include "Renderer.h"
#include "Lypant/Util/VertexArrays.h"
#include "Lypant/Util/Util.h"
#include "Lypant/Util/Textures.h"

namespace lypant
{
	Renderer::RendererData* Renderer::s_RendererData = nullptr;

	void Renderer::Init(uint32_t windowWidth, uint32_t windowHeight)
	{
		RenderCommand::Init();
		RenderCommand::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		s_RendererData = new RendererData();

		s_RendererData->WindowWidth = windowWidth;
		s_RendererData->WindowHeight = windowHeight;

		constexpr int bufferSize = sizeof(PointLight) * 40;
		s_RendererData->EnvironmentBuffer = new char[bufferSize];
		s_RendererData->EnvironmentUniformBuffer = new UniformBuffer(bufferSize, nullptr);

		s_RendererData->SkyboxShader = Shader::Load("shaders/Skybox.glsl");
		CreatePostProcessFrameBuffer();
		s_RendererData->PostProcessQuadVertexArray = util::VertexArrays::GetQuad();

		s_RendererData->PostProcessShader = Shader::Load("shaders/PostProcess.glsl");

		s_RendererData->BRDFIntegrationMap = util::Textures::GetBRDFIntegrationMap();

		s_RendererData->ShadowMapFrameBuffer = new FrameBuffer();
		s_RendererData->ShadowMapFrameBuffer->SetColorBufferToRender(-1);
		s_RendererData->CascadedShadowMapShader = Shader::Load("shaders/CascadedShadowMap.glsl");

		glm::vec4 borderColor(1.0f, 1.0f, 1.0f, 1.0f);

		s_RendererData->DirectionalLightShadowMaps = std::make_shared<Texture2DArray>(4096, 4096, 5, TextureWrappingOption::Clamp, true, reinterpret_cast<float*>(&borderColor));

		s_RendererData->DirectionalShadowMapShader = Shader::Load("shaders/DirectionalShadowMap.glsl");
		s_RendererData->SpotLightShadowMaps = std::make_shared<Texture2DArray>(1024, 1024, 8, TextureWrappingOption::Clamp, true, reinterpret_cast<float*>(&borderColor));

		s_RendererData->OmnidirectionalShadowMapShader = Shader::Load("shaders/OmnidirectionalShadowMap.glsl");
		s_RendererData->PointLightShadowMaps = std::make_shared<CubemapArray>(1024, 1024, 48, true);
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

	void Renderer::BeginScene(const Scene::SceneData& sceneData)
	{
		if (!s_RendererData->EnvironmentMap || s_RendererData->EnvironmentMap->GetPath() != sceneData.EnvironmentMap->GetPath())
		{
			s_RendererData->EnvironmentMap = sceneData.EnvironmentMap;
			s_RendererData->DiffuseIrradianceMap = util::CreateDiffuseIrradianceMap(s_RendererData->EnvironmentMap);
			s_RendererData->PrefilteredMap = util::CreatePreFilteredMap(s_RendererData->EnvironmentMap);
		}
		
		s_RendererData->AmbientStrength = sceneData.AmbientStrength;
		
		UpdateEnvironmentBuffers(sceneData);

		if (s_RendererData->MSAAFrameBuffer)
		{
			s_RendererData->MSAAFrameBuffer->Bind();
		}

		else
		{
			s_RendererData->PostProcessFrameBuffer->Bind();
		}

		RenderCommand::Clear();
	}

	void Renderer::EndScene()
	{
		// Render Environment map
		util::VertexArrays::GetCubemapCube()->Bind();
		s_RendererData->SkyboxShader->Bind();
		s_RendererData->EnvironmentMap->Bind(0);

		RenderCommand::DrawIndexed(util::VertexArrays::GetCubemapCube());

		if (s_RendererData->MSAAFrameBuffer)
		{
			s_RendererData->MSAAFrameBuffer->BlitToFrameBuffer(s_RendererData->PostProcessFrameBuffer, 0, 0, s_RendererData->MSAAFrameBuffer->GetColorBufferWidth(), s_RendererData->MSAAFrameBuffer->GetColorBufferHeight(), 0, 0, s_RendererData->MSAAFrameBuffer->GetColorBufferWidth(), s_RendererData->MSAAFrameBuffer->GetColorBufferHeight());
		}

		const std::shared_ptr<Texture2D>& sceneTexture = reinterpret_cast<const std::shared_ptr<Texture2D>&>(s_RendererData->PostProcessFrameBuffer->GetColorBuffer());

		if (s_RendererData->IsBloomEnabled)
		{
			CreateBloomTexture(sceneTexture);
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

		RenderCommand::SetDepthTest(true);
	}

	void Renderer::Submit(const Mesh& mesh, const glm::mat4& modelMatrix)
	{
		// TODO: I should probably move all the optional shader data to a uniform buffer including shadow and IBL data
		auto& shader = mesh.GetMaterial()->Shader;
		// TODO: have a get uniform value with a location parameter
		if (shader->GetUniformLocation("u_DiffuseIrradianceMap") != -1) s_RendererData->DiffuseIrradianceMap->Bind(shader->GetUniformValueInt("u_DiffuseIrradianceMap"));
		if (shader->GetUniformLocation("u_PreFilteredMap") != -1) s_RendererData->PrefilteredMap->Bind(shader->GetUniformValueInt("u_PreFilteredMap"));
		if (shader->GetUniformLocation("u_BRDFIntegrationMap") != -1) s_RendererData->BRDFIntegrationMap->Bind(shader->GetUniformValueInt("u_BRDFIntegrationMap"));

		mesh.GetVertexArray()->Bind();
		mesh.GetMaterial()->Bind();

		shader->SetUniformFloat("u_AmbientStrength", s_RendererData->AmbientStrength);
		shader->SetUniformMatrix4Float("u_ModelMatrix", (float*)&modelMatrix[0][0]);
		shader->SetUniformMatrix3Float("u_NormalMatrix", (float*)&(glm::transpose(glm::inverse(glm::mat3(modelMatrix))))[0][0]);

		if (shader->GetUniformLocation("u_DirectionalLightShadowMaps") != -1)
		{
			s_RendererData->DirectionalLightShadowMaps->Bind(shader->GetUniformValueInt("u_DirectionalLightShadowMaps"));
			for (int i = 0; i < s_RendererData->DirectionalLightSpaceMatrices.size(); i++)
			{
				shader->SetUniformMatrix4Float("u_DirectionalLightSpaceMatrices[" + std::to_string(i) + "]", &s_RendererData->DirectionalLightSpaceMatrices[i][0][0]);
				shader->SetUniformFloat("u_CascadePlaneDistances[" + std::to_string(i) + "]", s_RendererData->CascadePlaneDistances[i]);
			}
		}

		if (shader->GetUniformLocation("u_SpotLightShadowMaps") != -1)
		{
			s_RendererData->SpotLightShadowMaps->Bind(shader->GetUniformValueInt("u_SpotLightShadowMaps"));
			for (int i = 0; i < s_RendererData->SpotLightSpaceMatrices.size(); i++)
			{
				shader->SetUniformMatrix4Float("u_SpotLightSpaceMatrices[" + std::to_string(i) + "]", &s_RendererData->SpotLightSpaceMatrices[i][0][0]);
			}
		}

		if (shader->GetUniformLocation("u_PointLightShadowMaps") != -1)
		{
			s_RendererData->PointLightShadowMaps->Bind(shader->GetUniformValueInt("u_PointLightShadowMaps"));
		}

		RenderCommand::DrawIndexed(mesh.GetVertexArray());
	}

	void Renderer::BeginShadowPass(const Scene::SceneData& sceneData, LightType lightType)
	{
		s_RendererData->ShadowMapFrameBuffer->Bind();

		if (lightType == LightTypeDirectional)
		{
			s_RendererData->ShadowMapFrameBuffer->AttachDepthBuffer(s_RendererData->DirectionalLightShadowMaps);

			RenderCommand::Clear();
			RenderCommand::SetViewport(0, 0, s_RendererData->DirectionalLightShadowMaps->GetWidth(), s_RendererData->DirectionalLightShadowMaps->GetHeight());

			s_RendererData->CascadedShadowMapShader->Bind();
			for (int i = 0; i < sceneData.NumberOfDirectionalLights; i++)
			{
				if (!sceneData.DirectionalLightComponents[i].CastShadows) continue;
				CalculateDirectionalLightSpaceMatrices(sceneData.DirectionalLightComponents[i].Direction, *sceneData.Camera);
				for (int j = 0; j < s_RendererData->DirectionalLightSpaceMatrices.size(); j++)
				{
					s_RendererData->CascadedShadowMapShader->SetUniformMatrix4Float("u_DirectionalLightSpaceMatrices[" + std::to_string(j) + "]", &s_RendererData->DirectionalLightSpaceMatrices[j][0][0]);
				}
				break;
			}
		}

		else if (lightType == LightTypeSpot)
		{
			s_RendererData->ShadowMapFrameBuffer->AttachDepthBuffer(s_RendererData->SpotLightShadowMaps);

			RenderCommand::Clear();
			RenderCommand::SetViewport(0, 0, s_RendererData->SpotLightShadowMaps->GetWidth(), s_RendererData->SpotLightShadowMaps->GetHeight());

			glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.5f, 25.0f);

			int spotLightShadowIndex = 0;
			s_RendererData->DirectionalShadowMapShader->Bind();
			for (int i = 0; i < sceneData.NumberOfSpotLights && spotLightShadowIndex < s_RendererData->SpotLightShadowMaps->GetDepth(); i++)
			{
				const SpotLightComponent& light = sceneData.SpotLightComponents[i];
				if (!light.CastShadows) continue;

				glm::mat4 viewMatrix = glm::lookAt(glm::vec3(light.Position), glm::vec3(light.Position) + light.Direction, glm::vec3(0, 1, 0));
				s_RendererData->SpotLightSpaceMatrices[spotLightShadowIndex] = projectionMatrix * viewMatrix;
				s_RendererData->DirectionalShadowMapShader->SetUniformMatrix4Float("u_SpotLightSpaceMatrices[" + std::to_string(spotLightShadowIndex) + "]", &s_RendererData->SpotLightSpaceMatrices[spotLightShadowIndex][0][0]);
				spotLightShadowIndex++;
			}
		}

		else if (lightType == LightTypePoint)
		{
			s_RendererData->ShadowMapFrameBuffer->AttachDepthBuffer(s_RendererData->PointLightShadowMaps);

			RenderCommand::Clear();
			RenderCommand::SetViewport(0, 0, s_RendererData->PointLightShadowMaps->GetWidth(), s_RendererData->PointLightShadowMaps->GetHeight());

			glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.5f, 25.0f);

			int pointLightShadowIndex = 0;
			s_RendererData->OmnidirectionalShadowMapShader->Bind();
			for (int i = 0; i < sceneData.NumberOfPointLights && pointLightShadowIndex < s_RendererData->PointLightShadowMaps->GetDepth(); i++)
			{
				const PointLightComponent& light = sceneData.PointLightComponents[i];
				if (!light.CastShadows) continue;

				glm::mat4 viewMatrices[]
				{
					glm::lookAt(light.Position, light.Position + glm::vec3(1.0f,  0.0f,  0.0f),  glm::vec3(0.0f, -1.0f,  0.0f)),
					glm::lookAt(light.Position, light.Position + glm::vec3(-1.0f, 0.0f,  0.0f),  glm::vec3(0.0f, -1.0f,  0.0f)),
					glm::lookAt(light.Position, light.Position + glm::vec3(0.0f,  1.0f,  0.0f),  glm::vec3(0.0f,  0.0f,  1.0f)),
					glm::lookAt(light.Position, light.Position + glm::vec3(0.0f, -1.0f,  0.0f),  glm::vec3(0.0f,  0.0f, -1.0f)),
					glm::lookAt(light.Position, light.Position + glm::vec3(0.0f,  0.0f,  1.0f),  glm::vec3(0.0f, -1.0f,  0.0f)),
					glm::lookAt(light.Position, light.Position + glm::vec3(0.0f,  0.0f, -1.0f),  glm::vec3(0.0f, -1.0f,  0.0f))
				};

				for (int j = 0; j < 6; j++)
				{
					glm::mat4 lightSpaceMatrix = projectionMatrix * viewMatrices[j];
					s_RendererData->OmnidirectionalShadowMapShader->SetUniformMatrix4Float("u_PointLightSpaceMatrices[" + std::to_string(pointLightShadowIndex * 6 + j) + "]", &lightSpaceMatrix[0][0]);
					s_RendererData->OmnidirectionalShadowMapShader->SetUniformVec3Float("u_PointLightPositions[" + std::to_string(pointLightShadowIndex) + "]", reinterpret_cast<const float*>(&light.Position));
				}

				pointLightShadowIndex++;
			}
		}
	}

	void Renderer::EndShadowPass()
	{
		RenderCommand::SetViewport(0, 0, s_RendererData->WindowWidth, s_RendererData->WindowHeight);
	}

	void Renderer::SubmitForShadowPass(const Mesh& mesh, const glm::mat4& modelMatrix, LightType lightType, int count)
	{
		mesh.GetVertexArray()->Bind();

		if (lightType == LightTypeDirectional)
		{
			s_RendererData->CascadedShadowMapShader->SetUniformMatrix4Float("u_ModelMatrix", (float*)&modelMatrix[0][0]);
			RenderCommand::DrawIndexed(mesh.GetVertexArray());
		}

		else if (lightType == LightTypeSpot)
		{
			s_RendererData->DirectionalShadowMapShader->SetUniformMatrix4Float("u_ModelMatrix", (float*)&modelMatrix[0][0]);
			RenderCommand::DrawIndexedInstanced(mesh.GetVertexArray(), count);
		}

		else if (lightType == LightTypePoint)
		{
			s_RendererData->OmnidirectionalShadowMapShader->SetUniformMatrix4Float("u_ModelMatrix", (float*)&modelMatrix[0][0]);
			RenderCommand::DrawIndexedInstanced(mesh.GetVertexArray(), count);
		}
	}

	void Renderer::Submit(const Skybox& skybox)
	{
		skybox.GetVertexArray()->Bind();
		skybox.GetShader()->Bind();
		skybox.GetCubemap()->Bind(0);

		RenderCommand::DrawIndexed(skybox.GetVertexArray());
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

	void Renderer::UpdateEnvironmentBuffers(const Scene::SceneData& sceneData)
	{
		int totalPointLightSize = sizeof(PointLight) * sceneData.NumberOfPointLights;
		int pointLightsPadding = 256 - (totalPointLightSize % 256);

		int totalSpotLightSize = sizeof(SpotLight) * sceneData.NumberOfSpotLights;
		int spotLightsPadding = 256 - (totalSpotLightSize % 256);

		int totalDirectionalLightSize = sizeof(DirectionalLight) * sceneData.NumberOfDirectionalLights;
		int directionalLightPadding = 256 - (totalDirectionalLightSize % 256);

		int offset = 0;
		if (sceneData.PointLightComponents)
		{
			memcpy(&s_RendererData->EnvironmentBuffer[offset], sceneData.PointLightComponents, totalPointLightSize);
		}
		offset += totalPointLightSize + pointLightsPadding;

		int offsetForSpotLight = offset;
		if (sceneData.SpotLightComponents)
		{
			memcpy(&s_RendererData->EnvironmentBuffer[offset], sceneData.SpotLightComponents, totalSpotLightSize);
		}
		offset += totalSpotLightSize + spotLightsPadding;

		int offsetForDirectionalLight = offset;
		if (sceneData.DirectionalLightComponents)
		{
			memcpy(&s_RendererData->EnvironmentBuffer[offset], sceneData.DirectionalLightComponents, totalDirectionalLightSize);
		}
		offset += totalDirectionalLightSize + directionalLightPadding;

		int offsetForNumberOfLights = offset;
		constexpr int totalNumberOfLightsSize = sizeof(int) * 3;
		constexpr int totalNumberOfLightsPadding = 256 - totalNumberOfLightsSize;

		int numberOfLightsArray[]{ sceneData.NumberOfPointLights, sceneData.NumberOfSpotLights, sceneData.NumberOfDirectionalLights };
		memcpy(&s_RendererData->EnvironmentBuffer[offset], numberOfLightsArray, totalNumberOfLightsSize);

		offset += totalNumberOfLightsSize + totalNumberOfLightsPadding;
		int offsetForCamera = offset;

		memcpy(&s_RendererData->EnvironmentBuffer[offset], &sceneData.Camera->GetViewProjectionMatrix(), sizeof(glm::mat4));

		offset += sizeof(glm::mat4);

		memcpy(&s_RendererData->EnvironmentBuffer[offset], &sceneData.Camera->GetViewMatrix(), sizeof(glm::mat4));

		offset += sizeof(glm::mat4);

		memcpy(&s_RendererData->EnvironmentBuffer[offset], &sceneData.Camera->GetPosition(), sizeof(glm::vec3));

		offset += sizeof(glm::vec3);

		s_RendererData->EnvironmentUniformBuffer->BufferSubData(0, offset, s_RendererData->EnvironmentBuffer);

		if (sceneData.NumberOfPointLights != 0)
		{
			s_RendererData->EnvironmentUniformBuffer->BindRange(0, 0, totalPointLightSize);
		}

		if (sceneData.NumberOfSpotLights != 0)
		{
			s_RendererData->EnvironmentUniformBuffer->BindRange(1, offsetForSpotLight, totalSpotLightSize);
		}

		if (sceneData.NumberOfDirectionalLights != 0)
		{
			s_RendererData->EnvironmentUniformBuffer->BindRange(2, offsetForDirectionalLight, totalDirectionalLightSize);
		}

		s_RendererData->EnvironmentUniformBuffer->BindRange(3, offsetForNumberOfLights, totalNumberOfLightsSize);
		s_RendererData->EnvironmentUniformBuffer->BindRange(4, offsetForCamera, 2 * sizeof(glm::mat4) + sizeof(glm::vec3));
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

	std::vector<glm::vec4> Renderer::GetWorldPositionOfFrustumCorners(const glm::mat4& viewProjectionMatrix)
	{
		std::vector<glm::vec4> positions;
		positions.reserve(8);

		glm::mat4 inverseVP = glm::inverse(viewProjectionMatrix);

		for (int x = 0; x < 2; x++)
		{
			for (int y = 0; y < 2; y++)
			{
				for (int z = 0; z < 2; z++)
				{
					glm::vec4 position = inverseVP * glm::vec4(x * 2.0f - 1.0f, y * 2.0f - 1.0f, z * 2.0f - 1.0f, 1.0f);
					positions.push_back(position / position.w);
				}
			}
		}

		return positions;
	}

	void Renderer::CalculateDirectionalLightSpaceMatrices(const glm::vec3& lightDirection, const PerspectiveCamera& camera)
	{
		s_RendererData->CascadePlaneDistances = { camera.GetFarPlane() / 50.0f, camera.GetFarPlane() / 25.0f, camera.GetFarPlane() / 10.0f, camera.GetFarPlane() / 2.0f, camera.GetFarPlane()};

		CalculateDirectionalLightSpaceMatrix(lightDirection, camera, camera.GetNearPlane(), s_RendererData->CascadePlaneDistances[0], 0);

		for (int i = 1; i < s_RendererData->DirectionalLightSpaceMatrices.size(); i++)
		{
			CalculateDirectionalLightSpaceMatrix(lightDirection, camera, s_RendererData->CascadePlaneDistances[i - 1], s_RendererData->CascadePlaneDistances[i], i);
		}
	}

	void Renderer::CalculateDirectionalLightSpaceMatrix(const glm::vec3& lightDirection, const PerspectiveCamera& camera, float nearPlane, float farPlane, int cascade)
	{
		glm::mat4 cameraCascadedProjection = glm::perspective(camera.GetFovY(), camera.GetAspectRatio(), nearPlane, farPlane);
		std::vector<glm::vec4> frustumCorners = GetWorldPositionOfFrustumCorners(cameraCascadedProjection * camera.GetViewMatrix());

		glm::vec3 center(0.0f);
		for (const glm::vec4& corner : frustumCorners)
		{
			center += glm::vec3(corner);
		}
		center /= frustumCorners.size();

		// TODO: Current solution is not the best solution for edge cases
		glm::vec3 upVector(0.0f, 1.0f, 0.0f);
		if (glm::abs(glm::dot(upVector, lightDirection)) > 0.99f)
		{
			upVector = glm::vec3(0.0f, 0.0f, -1.0f);
		}

		float radius = farPlane * camera.GetAspectRatio() * glm::tan(camera.GetFovY() / 2.0) * 1.4f;
		
		float texelsPerUnitWorld = s_RendererData->DirectionalLightShadowMaps->GetWidth() / (radius * 2.0f);
		glm::mat4 scalar = glm::scale(glm::mat4(1.0f), glm::vec3(texelsPerUnitWorld));

		glm::mat4 texelSpaceMatrix = scalar * glm::lookAt(glm::vec3(0.0f), lightDirection, upVector);
		glm::mat4 inverseTexelSpaceMatrix = glm::inverse(texelSpaceMatrix);

		glm::vec4 centerTexelSpace = texelSpaceMatrix * glm::vec4(center, 1.0f);
		centerTexelSpace.x = glm::floor(centerTexelSpace.x);
		centerTexelSpace.y = glm::floor(centerTexelSpace.y);
		
		center = inverseTexelSpaceMatrix * centerTexelSpace;

		glm::mat4 viewMatrix = glm::lookAt(center - glm::normalize(lightDirection) * radius * 2.0f, center, upVector);

		constexpr float zMult = 10.0f;

		glm::mat4 projectionMatrix = glm::ortho(-radius, radius, -radius, radius, -radius * zMult, radius * zMult);

		s_RendererData->DirectionalLightSpaceMatrices[cascade] = projectionMatrix * viewMatrix;
	}
}
