#include "lypch.h"
#include "OpenGLContext.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace lypant
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) 
		: m_WindowHandle(windowHandle)
	{

	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		LY_CORE_VERIFY(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "GLAD could not initialize");
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}
