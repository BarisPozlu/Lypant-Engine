#include "lypch.h"
#include "Application.h"
#include "Event/WindowEvent.h"
#include "Layer.h"
#include "Lypant/ImGui/ImGuiLayer.h"
#include "Lypant/Renderer/Renderer.h"

namespace lypant
{
	Application* Application::m_Instance = nullptr;

	Application::Application() : m_Window(std::make_unique<Window>())
	{
		LY_CORE_ASSERT(!m_Instance, "Application already exists.");
		m_Instance = this;
		m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		while (m_Running)
		{
			RenderCommand::SetClearColor(0.2f, 0.5f, 0.8f, 1.0f);
			RenderCommand::Clear();

			for (Layer* layer : m_LayerStack)
			{
				layer->Tick();
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
