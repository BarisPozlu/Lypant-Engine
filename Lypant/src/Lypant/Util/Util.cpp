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

			std::shared_ptr<Image> cubemap = Cubemap::Create(1024, 1024, 4, params);
			std::shared_ptr<Image> image = Image2D::Create(path, params);

			std::shared_ptr<RenderTarget> renderTarget = RenderTarget::Create();
			renderTarget->AttachColorBuffer(cubemap);

			const auto& shader = Shader::Create("shaders/EquirectangularToCubemap.glsl");

			glm::mat4 viewMatrices[]
			{
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
			};

			std::unique_ptr<Subpass> subpass = Subpass::Create(renderTarget, Shader::Create("shaders/EquirectangularToCubemap.glsl"),
				{ { image, 0 } }, sizeof(viewMatrices), viewMatrices);

			auto& cmd = Renderer::GetRenderCommandBuffer();

			cmd.BeginSubpass(*subpass);
			
			cmd.DrawMesh(*MeshFactory::GetCubemapCube(), shader, 6);

			cmd.EndSubpass(*subpass);

			return cubemap;
		}
	}
}