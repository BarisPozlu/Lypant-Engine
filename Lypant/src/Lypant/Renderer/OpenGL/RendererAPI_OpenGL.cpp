#include "lypch.h"

#ifdef LYPANT_OPENGL

#include "Lypant/Renderer/RendererAPI.h"
#include <glad/glad.h>

namespace lypant
{
	void RendererAPI::Init()
	{
		glEnable(GL_CULL_FACE);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		glDepthFunc(GL_LEQUAL);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void RendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void RendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void RendererAPI::SetClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}

	void RendererAPI::SetDepthTest(bool value)
	{
		value ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
	}

	void RendererAPI::SetBlending(bool value)
	{
		value ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
	}

	void RendererAPI::SetBlendFunc(BlendFunc function)
	{
		switch (function)
		{
			case BlendFunc::SourceAlpha_OneMinusSourceAlpha:
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				break;
			case BlendFunc::One_One:
				glBlendFunc(GL_ONE, GL_ONE);
				break;
		}
	}

	void RendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void RendererAPI::DrawIndexedInstanced(const std::shared_ptr<VertexArray>& vertexArray, int instanceCount)
	{
		glDrawElementsInstanced(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr, instanceCount);
	}

}
#endif
