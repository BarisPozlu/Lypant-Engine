#include "lypch.h"
#include "Window.h"
#include <GLFW/glfw3.h>
#include "Lypant/Event/WindowEvent.h"
#include "Lypant/Event/KeyEvent.h"
#include "Lypant/Event/MouseEvent.h"

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
		m_Data.Height = props.Heigth;

		LY_CORE_INFO("Creating window \"{0}\"", m_Data.Title);

		if (!IsGlfwInitialized)
		{
			LY_CORE_VERIFY(glfwInit(), "GLFW could not initialize");
			IsGlfwInitialized = true;

			#ifdef LYPANT_DEBUG
			glfwSetErrorCallback(GLFWErrorCallback);
			#endif
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

		m_GraphicsContext.Hint();

		m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
		LY_CORE_ASSERT(m_Window, "Could not create GLFW Window.");
		m_GraphicsContext.Init(m_Window);

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowResizeEvent resizeEvent(width, height);
				WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;
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
					KeyPressEvent event(key, 0);
					data.EventCallback(event);
				}

				else if (action == GLFW_REPEAT)
				{
					KeyPressEvent event(key, 1);
					data.EventCallback(event);
				}

				else if (action == GLFW_RELEASE)
				{
					KeyReleaseEvent event(key);
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

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int codepoint)
			{
				KeyTypeEvent typeEvent(codepoint);
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.EventCallback(typeEvent);
			});

	}

	Window::~Window()
	{
		glfwDestroyWindow(m_Window);
	}

	void Window::Tick()
	{
		glfwPollEvents();
		m_GraphicsContext.SwapBuffers();
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
