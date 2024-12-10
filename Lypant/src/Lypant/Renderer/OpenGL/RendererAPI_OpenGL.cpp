#include "lypch.h"

#ifdef LYPANT_OPENGL

#include "Lypant/Renderer/RendererAPI.h"
#include <glad/glad.h>

namespace lypant
{
	void RendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void RendererAPI::SetClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}

	void RendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

}
#endif
