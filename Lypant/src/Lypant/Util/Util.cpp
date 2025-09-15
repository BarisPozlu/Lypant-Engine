#include <lypch.h>
#include "Util.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Lypant/Renderer/Renderer.h>
#include "MeshFactory.h"

namespace lypant
{
	namespace util
	{
		std::shared_ptr<Image> CreateCubemapFromEquirectangularImage(const std::string& path)
		{
			ImageParams params;
			params.FloatingImage = true;

			std::shared_ptr<Image> cubemap = Image::CreateCubemap(1024, 1024, 4, params);
			std::shared_ptr<Image> image = Image::CreateImage2D(path, params);

			std::shared_ptr<RenderTarget> renderTarget = RenderTarget::Create();
			renderTarget->AttachColorBuffer(cubemap);

			const auto& shader = Shader::Create("shaders/EquirectangularToCubemap.glsl");

			//glm::mat4 viewMatrices[]
			//{
			//	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			//	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			//	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			//	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			//	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			//	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			//};

			// NOTE: the vertices change winding order after the view matrix my guess is that their y coord is inverted and thats causing that
			glm::mat4 viewMatrices[]
			{
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, 1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, 1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f, 0.0f,  1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, 1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, 1.0f,  0.0f)),
			};

			std::unique_ptr<Subpass> subpass = Subpass::Create(renderTarget, false, shader,
				{ { image, 0 } }, sizeof(viewMatrices), viewMatrices);

			auto& cmd = Renderer::GetRenderCommandBuffer();

			cmd.BeginImmediateCommands();

			cmd.BeginSubpass(*subpass, true);
			
			cmd.DrawMesh(*MeshFactory::GetCubemapCube(), shader, 6, true);

			cmd.EndSubpass(*subpass, true);

			cmd.EndImmediateCommands();

			return cubemap;
		}

		std::shared_ptr<Image> CreateDiffuseIrradianceMap(const std::shared_ptr<Image>& source)
		{
			ImageParams params;
			params.FloatingImage = true;

			std::shared_ptr<Image> cubemap = Image::CreateCubemap(32, 32, 4, params);

			std::shared_ptr<RenderTarget> renderTarget = RenderTarget::Create();
			renderTarget->AttachColorBuffer(cubemap);

			const auto& shader = Shader::Create("shaders/CalculateDiffuseIrradianceMap.glsl");

			glm::mat4 viewMatrices[]
			{
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, 1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, 1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f, 0.0f,  1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, 1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, 1.0f,  0.0f)),
			};

			std::unique_ptr<Subpass> subpass = Subpass::Create(renderTarget, false, shader,
				{ { source, 0 } }, sizeof(viewMatrices), viewMatrices);

			auto& cmd = Renderer::GetRenderCommandBuffer();

			cmd.BeginImmediateCommands();

			cmd.BeginSubpass(*subpass, true);

			cmd.DrawMesh(*MeshFactory::GetCubemapCube(), shader, 6, true);

			cmd.EndSubpass(*subpass, true);

			cmd.EndImmediateCommands();

			return cubemap;
		}

		std::shared_ptr<Image> CreatePreFilteredMap(const std::shared_ptr<Image>& source)
		{
			ImageParams params;
			params.FloatingImage = true;
			params.GenerateMipMap = true;
			params.CreateViewsPerMipMap = true;

			std::shared_ptr<Image> cubemap = Image::CreateCubemap(128, 128, 4, params);

			std::shared_ptr<RenderTarget> renderTarget = RenderTarget::Create();
			renderTarget->AttachColorBuffer(cubemap);

			const auto& shader = Shader::Create("shaders/CalculatePreFilteredMap.glsl");

			glm::mat4 viewMatrices[]
			{
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, 1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, 1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f, 0.0f,  1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, 1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, 1.0f,  0.0f)),
			};

			std::unique_ptr<Subpass> subpass = Subpass::Create(renderTarget, false, shader,
				{ { source, 0 } }, sizeof(viewMatrices), viewMatrices);

			auto& cmd = Renderer::GetRenderCommandBuffer();

			cmd.BeginImmediateCommands();

			constexpr int mipmapLevels = 5;

			for (int level = 0; level < mipmapLevels; level++)
			{
				int mipWidth = 128 * glm::pow(0.5, level);
				int mipHeight = 128 * glm::pow(0.5, level);

				renderTarget->AttachColorBuffer(cubemap, level);

				cmd.BeginSubpass(*subpass, true);

				float roughness = static_cast<float>(level) / (mipmapLevels - 1);
				cmd.PushData(&roughness, sizeof(roughness), shader, ShaderStageFlagsFragment, true);

				cmd.DrawMesh(*util::MeshFactory::GetCubemapCube(), shader, 6, true);

				cmd.EndSubpass(*subpass, true);
			}

			cmd.EndImmediateCommands();

			return cubemap;
		}
	}
}