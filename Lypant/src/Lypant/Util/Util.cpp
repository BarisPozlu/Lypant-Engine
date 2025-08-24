#include <lypch.h>
#include "Util.h"
#include <Lypant/Renderer/Renderer.h>
#include "MeshFactory.h"

namespace lypant
{
	namespace util
	{
		std::shared_ptr<Image> CreateCubemapFromEquirectangularImage(const std::string& path)
		{
			//ImageParams params;
			//params.FloatingImage = true;

			//std::shared_ptr<Image> cubemap = Cubemap::Create(1024, 1024, 3, params);
			//std::shared_ptr<Image> image = Image2D::Create(path, params);

			//std::shared_ptr<RenderTarget> renderTarget = RenderTarget::Create();
			//renderTarget->AttachColorBuffer(cubemap);

			//const auto& shader = Shader::Create("shaders/EquirectangularToCubemap.glsl");

			//std::unique_ptr<Subpass> subpass = Subpass::Create(renderTarget, Shader::Create("shaders/EquirectangularToCubemap.glsl"), { { image, 0 } });

			//auto& renderCommandBuffer = Renderer::GetRenderCommandBuffer();

			//renderCommandBuffer.BeginSubpass(*subpass);

			//// draw the cube
			//Renderer::SubmitMesh(*MeshFactory::GetCubemapCube(), shader);

			//renderCommandBuffer.EndSubpass(*subpass);

			//return cubemap;

			return nullptr;
		}
	}
}