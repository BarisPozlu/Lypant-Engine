#include "lypch.h"
#include "Renderer.h"

namespace lypant
{
	Renderer::RendererData* Renderer::s_Data = nullptr;
	RenderCommandBuffer* Renderer::s_Cmd = nullptr;

	void Renderer::Init()
	{
		s_Data = new RendererData();

		s_Cmd = RenderCommandBuffer::Create();

		std::shared_ptr<RenderTarget> renderTarget = RenderTarget::CreateDefault();

		s_Data->TestShader = Shader::Create("shaders/VulkanTest.glsl");

		s_Data->TestImage = Image2D::Create("textures/vulkan-test.png", ImageParams());

		s_Data->TestPass = Subpass::Create(renderTarget, s_Data->TestShader, { { s_Data->TestImage, 0 } });
	}

	void Renderer::Shutdown()
	{
		delete s_Cmd;
		delete s_Data;
	}

	void Renderer::BeginRendering()
	{
		s_Cmd->BeginCommands();
	}

	void Renderer::EndRendering()
	{
		s_Cmd->EndSubpass(*s_Data->TestPass);
		s_Cmd->EndCommands();
	}

	void Renderer::BeginScene()
	{
		s_Cmd->BeginSubpass(*s_Data->TestPass);
	}

	void Renderer::EndScene()
	{
		//s_Cmd->EndSubpass(*s_Data->TestPass);
	}

	RenderCommandBuffer& Renderer::GetRenderCommandBuffer()
	{
		return *s_Cmd;
	}
}
