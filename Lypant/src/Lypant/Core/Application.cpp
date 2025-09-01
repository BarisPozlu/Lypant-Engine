#include "lypch.h"
#include "Application.h"
#include "Lypant/Event/WindowEvent.h"
#include "Layer.h"
#include "Lypant/ImGui/ImGuiLayer.h"
#include "Lypant/Renderer/Renderer.h"
#include <GLFW/glfw3.h> // temp
#include "Lypant/Input/Input.h" // temp
//#include "Lypant/Util/VertexArrays.h"
//#include "Lypant/Util/Textures.h"
#include <Lypant/Util/MeshFactory.h>

namespace lypant
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		LY_CORE_ASSERT(!s_Instance, "Application already exists.");
		s_Instance = this;

		Log::Init();

		GraphicsContext::SetGraphicsAPI(GraphicsAPI::Vulkan);

		m_Window = std::make_unique<Window>();
		m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

		//util::VertexArrays::Create();
		//util::Textures::Create();

		Renderer::Init();

		m_ImGuiLayer = ImGuiLayer::Create();
		PushOverlay(m_ImGuiLayer);

		Input::Init();
	}

	Application::~Application()
	{
		Input::Shutdown();
		Renderer::Shutdown();
		//util::Textures::Destroy();
		//util::VertexArrays::Destroy();
	}

	void Application::Run()
	{
		while (m_Running)
		{
			float time = glfwGetTime();
			float deltaTime = time - m_LastFrameTime;
			m_LastFrameTime = time;

			Renderer::BeginRendering();

			// TODO: Move
			Renderer::BeginScene();

			if (!m_Minimized)
			{
				Input::Tick(deltaTime);

				for (Layer* layer : m_LayerStack)
				{
					layer->Tick(deltaTime);
				}
			}

			// TODO: Move
			Renderer::EndScene();

			m_ImGuiLayer->Begin();

			for (Layer* layer : m_LayerStack)
			{
				layer->OnImGuiRender();
			}

			m_ImGuiLayer->End();

			Renderer::EndRendering();

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
				//Renderer::OnWindowResize(resizeEvent.GetWidth(), resizeEvent.GetHeight());
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
