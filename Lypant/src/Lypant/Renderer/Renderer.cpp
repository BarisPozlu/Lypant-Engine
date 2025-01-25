#include "lypch.h"
#include "Renderer.h"

namespace lypant
{
	Renderer::SceneData Renderer::s_SceneData;

	// getting rid of the vpointer, because it is not needed in the gpu.
	static constexpr int s_PurePointLightSize = sizeof(PointLight) - sizeof(void*);
	static constexpr int s_PureSpotLightSize = sizeof(SpotLight) - sizeof(void*);
	static constexpr int s_PureDirectionalLightSize = sizeof(DirectionalLight) - sizeof(void*);
	static constexpr int s_BufferSize = s_PurePointLightSize * 10 + s_PureDirectionalLightSize * 10;

	void Renderer::Init()
	{
		RenderCommand::Init();

		s_SceneData.LightBuffer = new char[s_BufferSize];
		s_SceneData.LightUniformBuffer = std::make_unique<UniformBuffer>(s_BufferSize, nullptr);
	}

	void Renderer::Shutdown()
	{
		s_SceneData.Lights.swap(std::vector<std::shared_ptr<Light>>());
		s_SceneData.Camera.reset();
		s_SceneData.LightUniformBuffer.release();
		delete[] s_SceneData.LightBuffer;
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(const std::shared_ptr<PerspectiveCamera>& camera, const std::vector<std::shared_ptr<Light>>& lights)
	{
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
			memcpy(&s_SceneData.LightBuffer[offset], ((char*)s_SceneData.Lights.at(i).get()) + 8, s_PurePointLightSize);
			offset += s_PurePointLightSize;
		}

		offset += pointLightsPadding;
		int offsetForSpotLight = offset;

		for (; i < numberOfLights[LightTypePoint] + numberOfLights[LightTypeSpot]; i++)
		{
			memcpy(&s_SceneData.LightBuffer[offset], ((char*)s_SceneData.Lights.at(i).get()) + 8, s_PureSpotLightSize);
			offset += s_PureSpotLightSize;
		}

		offset += spotLightsPadding;
		int offsetForDirectionalLight = offset;

		for (; i < s_SceneData.Lights.size(); i++)
		{
			memcpy(&s_SceneData.LightBuffer[offset], ((char*)s_SceneData.Lights.at(i).get()) + 8, s_PureDirectionalLightSize);
			offset += s_PureDirectionalLightSize;
		}
		
		offset += directionalLightPadding;
		int offsetForNumberOfLights = offset;

		constexpr int totalNumberOfLightsSize = sizeof(int) * 3;

		memcpy(&s_SceneData.LightBuffer[offset], numberOfLights, totalNumberOfLightsSize);
		
		offset += totalNumberOfLightsSize;
		
		s_SceneData.LightUniformBuffer->BufferSubData(0, offset, s_SceneData.LightBuffer);

		if (numberOfLights[LightTypePoint] != 0)
		{
			s_SceneData.LightUniformBuffer->BindRange(0, 0, totalPointLightSize);
		}

		if (numberOfLights[LightTypeSpot] != 0)
		{
			s_SceneData.LightUniformBuffer->BindRange(1, offsetForSpotLight, totalSpotLightSize);
		}

		if (numberOfLights[LightTypeDirectional] != 0)
		{
			s_SceneData.LightUniformBuffer->BindRange(2, offsetForDirectionalLight, totalDirectionalLightSize);
		}

		s_SceneData.LightUniformBuffer->BindRange(3, offsetForNumberOfLights, totalNumberOfLightsSize);
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
