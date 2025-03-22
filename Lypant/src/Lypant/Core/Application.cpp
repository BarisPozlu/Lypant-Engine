#include "lypch.h"
#include "Application.h"
#include "Lypant/Event/WindowEvent.h"
#include "Layer.h"
#include "Lypant/ImGui/ImGuiLayer.h"
#include "Lypant/Renderer/Renderer.h"
#include <GLFW/glfw3.h> // temp
#include "Lypant/Input/Input.h" // temp

namespace lypant
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		LY_CORE_ASSERT(!s_Instance, "Application already exists.");
		s_Instance = this;

		Log::Init();

		m_Window = std::make_unique<Window>();
		m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

		Renderer::Init(m_Window->GetWidth(), m_Window->GetHeight());

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		Input::Init();
	}

	Application::~Application()
	{
		Input::Shutdown();
		Renderer::Shutdown();
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
