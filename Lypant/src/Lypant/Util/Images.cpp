#include "lypch.h"
#include "Images.h"
#include "Lypant/Renderer/Renderer.h"
#include "Lypant/Renderer/Image.h"
#include "Lypant/Renderer/Shader.h"
#include "MeshFactory.h"

namespace lypant
{
	namespace util
	{
		static std::shared_ptr<Image> CreateWhite4Channel1x1()
		{
			uint32_t color = 0xFFFFFFFF;
			std::shared_ptr<Image> image = Image::CreateImage2D(1, 1, 4, reinterpret_cast<unsigned char*>(&color), {});
			return image;
		}

		static std::shared_ptr<Image> CreateWhite1Channel1x1()
		{
			uint32_t color = 0xFFFFFFFF;
			std::shared_ptr<Image> image = Image::CreateImage2D(1, 1, 1, reinterpret_cast<unsigned char*>(&color), {});
			return image;
		}

		static std::shared_ptr<Image> CreateBRDFIntegrationMap()
		{
			ImageParams params;
			params.FloatingImage = true;
			params.SamplerSpec.WrappingOption = ImageWrappingOption::ClampEdge;
			std::shared_ptr<Image> BRDFIntegrationMap = Image::CreateImage2DRenderTarget(512, 512, 2, params, true);

			std::shared_ptr<RenderTarget> renderTarget = RenderTarget::Create();
			renderTarget->AttachColorBuffer(BRDFIntegrationMap);

			std::shared_ptr<Shader> shader = Shader::Create("shaders/CalculateBRDFIntegrationMap.glsl");

			std::unique_ptr<Subpass> subpass = Subpass::Create(renderTarget, false, shader, {}, 0, nullptr);
			
			auto& cmd = Renderer::GetRenderCommandBuffer();

			cmd.BeginImmediateCommands();

			cmd.BeginSubpass(*subpass, true);

			cmd.DrawMesh(*MeshFactory::GetQuad(), shader, 1, true);

			cmd.EndSubpass(*subpass, true);

			cmd.EndImmediateCommands();

			return BRDFIntegrationMap;
		}

		void Images::Create()
		{
			s_White4Channel1x1 = CreateWhite4Channel1x1();
			s_White1Channel1x1 = CreateWhite1Channel1x1();
			s_BRDFIntegrationMap = CreateBRDFIntegrationMap();
		}

		void Images::Destroy()
		{
			s_White4Channel1x1.reset();
			s_White1Channel1x1.reset();
			s_BRDFIntegrationMap.reset();
		}
	}
}
