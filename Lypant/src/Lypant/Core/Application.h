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

		inline static Application& Get() { return *s_Instance; }
		inline static Window& GetWindow() { return *s_Instance->m_Window; }
		inline static const std::unique_ptr<GraphicsContext>& GetGraphicsContext() { return GetWindow().GetGraphicsContext(); }

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
	private:
		static Application* s_Instance;
	private:
		std::unique_ptr<Window> m_Window;
		std::unique_ptr<class Renderer> m_Renderer;
		//ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0;
		bool m_Running = true;
		bool m_Minimized = false;
	};

	Application* CreateApplication();
}
