#include "lypch.h"
#include "Application.h"

namespace lypant
{
	Application::Application() : m_Window(std::make_unique<Window>())
	{
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
}
