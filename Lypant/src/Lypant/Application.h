#pragma once

#include "lypch.h"
#include "Core.h"
#include "Window.h"

namespace lypant
{
	class LYPANT_API Application
	{
	public:
		Application();
		virtual ~Application();

		virtual void Run();

		void OnEvent(Event& event);
	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

	Application* CreateApplication();
}
