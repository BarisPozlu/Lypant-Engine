#pragma once

#include "lypch.h"
#include "Core.h"
#include "Window.h"
#include "LayerStack.h"

namespace lypant
{
	class LYPANT_API Application
	{
	public:
		Application();
		virtual ~Application();

		virtual void Run();

		void OnEvent(Event& event);

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);
	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
	};

	Application* CreateApplication();
}
