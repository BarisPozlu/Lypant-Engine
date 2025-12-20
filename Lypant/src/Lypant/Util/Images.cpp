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

			RenderTargetOperation op;
			op.ColorBufferStoreOp = AttachmentStoreOperation::Store;

			std::unique_ptr<Subpass> subpass = Subpass::Create(renderTarget, op, shader, {}, 0, nullptr);
			subpass->Submit(*MeshFactory::GetQuad(), glm::mat4(1.0f));
			
			auto& cmd = Renderer::GetRenderCommandBuffer();

			cmd.BeginImmediateCommands();

			cmd.ExecuteSubpass(*subpass, true);

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
