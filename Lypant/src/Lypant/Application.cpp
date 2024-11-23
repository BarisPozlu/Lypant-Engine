#include "lypch.h"
#include "Application.h"
#include "Event/WindowEvent.h"
#include "Layer.h"

namespace lypant
{
	Application::Application() : m_Window(std::make_unique<Window>())
	{
		m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{		
		while (m_Running)
		{
			for (Layer* layer : m_LayerStack)
			{
				layer->Tick();
			}

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


	void Application::PopLayer(Layer* layer)
	{
		m_LayerStack.PopLayer(layer);
	}
}
