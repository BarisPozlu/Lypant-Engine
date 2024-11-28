#include "lypch.h"
#include "Window.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Event/WindowEvent.h"
#include "Event/KeyEvent.h"
#include "Event/MouseEvent.h"

namespace lypant
{
	static bool IsGlfwInitialized = false;

	static void GLFWErrorCallback(int error_code, const char* description)
	{
		LY_CORE_ERROR("GLFW Error ({0}): {1}", error_code, description);
	}

	Window::Window(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Heigth = props.Heigth;

		LY_CORE_INFO("Creating window \"{0}\"", m_Data.Title);

		if (!IsGlfwInitialized)
		{
			LY_CORE_VERIFY(glfwInit(), "GLFW could not initialize");
			IsGlfwInitialized = true;
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		//TODO: Implement debugging here.

		m_Window = glfwCreateWindow(m_Data.Width, m_Data.Heigth, m_Data.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		LY_CORE_VERIFY(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "GLAD could not initialize");
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowResizeEvent resizeEvent(width, height);
				WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Heigth = height;
				data.EventCallback(resizeEvent);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowCloseEvent closeEvent;
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.EventCallback(closeEvent);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				
				if (action == GLFW_PRESS)
				{
					KeyPressEvent event(scancode, 0);
					data.EventCallback(event);
				}

				else if (action == GLFW_REPEAT)
				{
					KeyPressEvent event(scancode, 1);
					data.EventCallback(event);
				}

				else if (action == GLFW_RELEASE)
				{
					KeyReleaseEvent event(scancode);
					data.EventCallback(event);
				}
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				
				if (action == GLFW_PRESS)
				{
					MouseButtonPressEvent event(button);
					data.EventCallback(event);
				}

				else if (action == GLFW_RELEASE)
				{
					MouseButtonReleaseEvent event(button);
					data.EventCallback(event);
				}
			});


		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset)
			{
				MouseScrollEvent scrollEvent(xoffset, yoffset);
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.EventCallback(scrollEvent);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
			{
				MouseMoveEvent moveEvent(xpos, ypos);
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.EventCallback(moveEvent);
			});

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
