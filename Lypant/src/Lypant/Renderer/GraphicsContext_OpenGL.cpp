#include "lypch.h"

#ifdef LYPANT_OPENGL

#include "GraphicsContext.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace lypant
{
	void GraphicsContext::Init(void* windowHandle)
	{
		m_WindowHandle = windowHandle;
		glfwMakeContextCurrent((GLFWwindow*)m_WindowHandle);
		LY_CORE_VERIFY(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "GLAD could not initialize");
	}

	void GraphicsContext::SwapBuffers() const
	{
		glfwSwapBuffers((GLFWwindow*)m_WindowHandle);
	}
}

#endif
