#include "lypch.h"
#include "Application.h"
#include "Lypant/Event/WindowEvent.h"
#include "Layer.h"
#include "Lypant/ImGui/ImGuiLayer.h"
#include "Lypant/Renderer/Renderer.h"
#include "Lypant/Renderer/Shader.h" // temp
#include "Lypant/Camera/PerspectiveCamera.h" // temp
#include "Lypant/Camera/EditorPerspectiveCamera.h" // temp
#include "glm/glm.hpp" // temp
#include <GLFW/glfw3.h> // temp
#include "Lypant/Renderer/Texture.h" // temp
#include "Lypant/Input/Input.h" // temp

namespace lypant
{
	Application* Application::s_Instance = nullptr;

	Application::Application() : m_Window(std::make_unique<Window>())
	{
		LY_CORE_ASSERT(!s_Instance, "Application already exists.");
		s_Instance = this;

		m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		Input::Init();
	}

	Application::~Application()
	{
		Input::Shutdown();
	}

	void Application::Run()
	{
		while (m_Running)
		{
			float time = glfwGetTime();
			float deltaTime = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				Input::Tick(deltaTime);

				for (Layer* layer : m_LayerStack)
				{
					layer->Tick(deltaTime);
				}
			}

			m_ImGuiLayer->Begin();

			for (Layer* layer : m_LayerStack)
			{
				layer->OnImGuiRender();
			}

			m_ImGuiLayer->End();

			m_Window->Tick();
		}
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& closeEvent)
			{
				m_Running = false;
				return true;
			});

		dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& resizeEvent)
			{
				if (resizeEvent.GetWidth() == 0 || resizeEvent.GetHeight() == 0)
				{
					m_Minimized = true;
					return false;
				}

				m_Minimized = false;
				Renderer::OnWindowResize(resizeEvent.GetWidth(), resizeEvent.GetHeight());
				return false;
			});

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			if (event.IsHandled())
			{
				break;
			}
			(*(--it))->OnEvent(event);
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}


	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
	}
}
