#include "lypch.h"
#include "Window.h"
#include "GLFW/glfw3.h"

namespace lypant
{
	static bool IsGlfwInitialized = false;

	Window::Window(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Heigth = props.Heigth;

		LY_CORE_INFO("Creating window {0}", m_Data.Title);

		if (!IsGlfwInitialized)
		{
			bool success = glfwInit();
			LY_CORE_ASSERT(success, "GLFW could not initialize");
			IsGlfwInitialized = true;
		}

		//TODO: Implement debugging here.
		m_Window = glfwCreateWindow(m_Data.Width, m_Data.Heigth, m_Data.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_Window);
	}

	void Window::Tick()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void Window::SetVSync(bool enabled)
	{
		if (enabled)
		{
			glfwSwapInterval(1);
		}
		else
		{
			glfwSwapInterval(0);
		}

		m_Data.IsVSync = enabled;
	}
}
