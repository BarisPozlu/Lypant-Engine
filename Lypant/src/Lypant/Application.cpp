#include "lypch.h"
#include "Application.h"
#include "Event/WindowEvent.h"

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
	}
}
