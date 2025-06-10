#include "lypch.h"

#ifdef LYPANT_OPENGL

#include "Lypant/Renderer/GraphicsContext.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace lypant
{
	static void APIENTRY DebugMessageCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char* message, const void* userParam)
	{
		if (id == 131185 || type == GL_DEBUG_TYPE_PERFORMANCE) return;

		LY_CORE_ERROR("id: {0}, message: {1}", id, message);
		LY_CORE_ASSERT(false, "OpenGL debug callback!");
	}

	void GraphicsContext::Hint()
	{
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		#ifdef LYPANT_DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
		#endif
	}

	void GraphicsContext::Init(void* windowHandle)
	{
		m_WindowHandle = windowHandle;
		glfwMakeContextCurrent((GLFWwindow*)m_WindowHandle);
		LY_CORE_VERIFY(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "GLAD could not initialize");

		#ifdef LYPANT_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(DebugMessageCallBack, nullptr);
		glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
		#endif 
	}

	void GraphicsContext::SwapBuffers() const
	{
		glfwSwapBuffers((GLFWwindow*)m_WindowHandle);
	}
}

#endif
