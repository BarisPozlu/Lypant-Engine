#pragma once

#include "lypch.h"
#include "Core.h"
#include "Window.h"
#include "LayerStack.h"

namespace lypant
{
	class ImGuiLayer;

	class LYPANT_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& event);

		inline static Application& Get() { return *m_Instance; }
		inline Window& GetWindow() { return *m_Window; }

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;
		static Application* m_Instance;
		float m_LastFrameTime = 0;
		bool m_Running = true;
	};

	Application* CreateApplication();
}
