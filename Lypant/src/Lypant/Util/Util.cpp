#include "lypch.h"
#include "Util.h"
#include "Lypant/Renderer/Texture.h"
#include "Lypant/Renderer/Buffer.h"
#include "Lypant/Renderer/Renderer.h"
#include "Lypant/Core/Application.h"
#include "VertexArrays.h"

namespace lypant
{
	namespace util
	{
		std::shared_ptr<Cubemap> CreateDiffuseIrradianceMap(const std::shared_ptr<Cubemap>& source)
		{
			const auto& vertexArray = VertexArrays::GetCubemapCube();
			std::shared_ptr<Shader> shader = Shader::Load("shaders/CalculateDiffuseIrradianceMap.glsl");
			shader->SetUniformInt("u_EnvironmentMap", 0);

			std::shared_ptr<Cubemap> diffuseIrradianceMap = std::make_shared<Cubemap>(32, 32, false, true);

			FrameBuffer framebuffer;
			RenderCommand::SetViewport(0, 0, 32, 32);

			glm::mat4 viewMatrices[]
			{
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
			};

			vertexArray->Bind();
			shader->Bind();
			source->Bind(0);

			constexpr int numberOfFaces = 6;

			for (int i = 0; i < numberOfFaces; i++)
			{
				framebuffer.AttachColorBuffer(diffuseIrradianceMap, 0, i);
				shader->SetUniformMatrix4Float("u_ViewMatrix", (float*)&viewMatrices[i]);

				RenderCommand::DrawIndexed(vertexArray);
			}

			shader->Unbind(); // causes a debug message callback if not done, not sure why, must be the drivers doing some stuff in the background
			FrameBuffer::BindDefaultFrameBuffer();
			RenderCommand::SetViewport(0, 0, Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());

			return diffuseIrradianceMap;
		}

		std::shared_ptr<Cubemap> CreatePreFilteredMap(const std::shared_ptr<Cubemap>& source)
		{
			const auto& vertexArray = VertexArrays::GetCubemapCube();
			std::shared_ptr<Shader> shader = Shader::Load("shaders/CalculatePrefilteredMap.glsl");
			shader->SetUniformInt("u_EnvironmentMap", 0);

			std::shared_ptr<Cubemap> preFilteredMap = std::make_shared<Cubemap>(128, 128, true, true);

			FrameBuffer framebuffer;

			glm::mat4 viewMatrices[]
			{
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
			};

			vertexArray->Bind();
			shader->Bind();
			source->Bind(0);

			constexpr int mipmapLevels = 5;
			constexpr int numberOfFaces = 6;

			for (int level = 0; level < mipmapLevels; level++)
			{
				int mipWidth = 128 * glm::pow(0.5, level);
				int mipHeight = 128 * glm::pow(0.5, level);
				RenderCommand::SetViewport(0, 0, mipWidth, mipHeight);

				float roughness = (float)level / (mipmapLevels - 1);
				shader->SetUniformFloat("u_Roughness", roughness);

				for (int i = 0; i < numberOfFaces; i++)
				{
					framebuffer.AttachColorBuffer(preFilteredMap, level, i);
					shader->SetUniformMatrix4Float("u_ViewMatrix", (float*)&viewMatrices[i]);

					RenderCommand::DrawIndexed(vertexArray);
				}
			}

			shader->Unbind(); // causes a debug message callback if not done, not sure why, must be the drivers doing some stuff in the background
			FrameBuffer::BindDefaultFrameBuffer();
			RenderCommand::SetViewport(0, 0, Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());

			return preFilteredMap;
		}

		std::shared_ptr<Cubemap> CreateCubemapFromEquirectangularTexture(const std::string& path)
		{
			const auto& vertexArray = VertexArrays::GetCubemapCube();
			std::shared_ptr<Shader> shader = Shader::Load("shaders/EquirectangularToCubemap.glsl");

			std::shared_ptr<Texture2D> equirectangularTexture = Texture2D::Load(path, false, false, true);

			std::shared_ptr<Cubemap> cubemap = std::make_shared<Cubemap>(1024, 1024, false, true, path);

			FrameBuffer frameBuffer;
			RenderCommand::SetViewport(0, 0, 1024, 1024);

			glm::mat4 viewMatrices[]
			{
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
			};

			vertexArray->Bind();
			shader->Bind();
			equirectangularTexture->Bind(0);

			constexpr int numberOfFaces = 6;

			for (int i = 0; i < numberOfFaces; i++)
			{
				frameBuffer.AttachColorBuffer(cubemap, 0, i);
				shader->SetUniformMatrix4Float("u_ViewMatrix", (float*)&viewMatrices[i]);

				RenderCommand::DrawIndexed(vertexArray);
			}

			shader->Unbind(); // causes a debug message callback if not done, not sure why, must be the drivers doing some stuff in the background
			FrameBuffer::BindDefaultFrameBuffer();
			RenderCommand::SetViewport(0, 0, Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());

			return cubemap;
		}
	}
}
