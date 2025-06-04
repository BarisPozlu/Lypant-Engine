#include "lypch.h"
#include "Textures.h"
#include "Lypant/Core/Application.h"
#include "Lypant/Renderer/Renderer.h"
#include "Lypant/Renderer/Texture.h"
#include "Lypant/Renderer/Shader.h"
#include "Lypant/Renderer/VertexArray.h"
#include "VertexArrays.h"

namespace lypant
{
	namespace util
	{
		static std::shared_ptr<Texture2D> CreateWhite3Channel1x1()
		{
			uint32_t color = 0xFFFFFFFF;
			std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>(1, 1, reinterpret_cast<unsigned char*>(&color), false);
			return texture;
		}

		static std::shared_ptr<Texture2D> CreateWhite1Channel1x1()
		{
			uint32_t color = 0xFFFFFFFF;
			std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>(1, 1, reinterpret_cast<unsigned char*>(&color), false, false, 1);
			return texture;
		}

		static std::shared_ptr<Texture2D> CreateBRDFIntegrationMap()
		{
			std::shared_ptr<Texture2D> BRDFIntegrationMap = std::make_shared<Texture2D>(512, 512, nullptr, true, true, 2, TextureWrappingOption::Clamp);
			std::shared_ptr<Shader> shader = Shader::Load("shaders/CalculateBRDFIntegrationMap.glsl");

			RenderCommand::SetViewport(0, 0, 512, 512);

			FrameBuffer framebuffer;

			framebuffer.AttachColorBuffer(BRDFIntegrationMap);

			VertexArrays::GetQuad()->Bind();
			shader->Bind();
			RenderCommand::DrawIndexed(VertexArrays::GetQuad());

			FrameBuffer::BindDefaultFrameBuffer();
			RenderCommand::SetViewport(0, 0, ::lypant::Application::Get().GetWindow().GetWidth(), ::lypant::Application::Get().GetWindow().GetHeight());

			return BRDFIntegrationMap;
		}

		void Textures::Create()
		{
			s_White3Channel1x1 = CreateWhite3Channel1x1();
			s_White1Channel1x1 = CreateWhite1Channel1x1();
			s_BRDFIntegrationMap = CreateBRDFIntegrationMap();
		}

		void Textures::Destroy()
		{
			s_White3Channel1x1.reset();
			s_BRDFIntegrationMap.reset();
		}

	}
}
