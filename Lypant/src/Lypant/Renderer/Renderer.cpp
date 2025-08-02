#include "lypch.h"
#include "Renderer.h"

namespace lypant
{
	Renderer::RendererData* Renderer::s_Data = nullptr;

	void Renderer::Init()
	{
		s_Data = new RendererData();

		s_Data->Cmd = RenderCommandBuffer::Create();
	}

	void Renderer::Shutdown()
	{
		delete s_Data;
	}

	void Renderer::BeginRendering()
	{
		s_Data->Cmd->BeginCommands();

		//TODO: Remove
		s_Data->Cmd->SetRenderTargetToDefault();
	}

	void Renderer::EndRendering()
	{
		s_Data->Cmd->EndCommands();
	}

	RenderCommandBuffer& Renderer::GetRenderCommandBuffer()
	{
		return *s_Data->Cmd;
	}
}
