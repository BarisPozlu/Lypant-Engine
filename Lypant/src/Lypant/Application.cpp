#include "lypch.h"
#include "Application.h"
#include "Event/WindowEvent.h"
#include "Layer.h"
#include "Lypant/ImGui/ImGuiLayer.h"
#include "Lypant/Renderer/Renderer.h"
#include "Lypant/Renderer/Shader.h" // temp

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
		//temp
		Shader shader("../Lypant/src/Lypant/Renderer/OpenGL/Shaders/VertexShader.shader", "../Lypant/src/Lypant/Renderer/OpenGL/Shaders/FragmentShader.shader");
		shader.Bind();

		float color[]
		{
			0.8f, 0.2f, 0.3f, 1.0f
		};

		shader.SetVec4FloatUniform("u_Color", color);

		std::shared_ptr<VertexArray> vertexArray = std::make_shared<VertexArray>();
		vertexArray->Bind();
		
		float vertexData[]
		{
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.25f, 0.0f
		};

		std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>(vertexData, sizeof(float) * 3 * 3);

		BufferLayout layout
		{
			{ShaderDataType::Float3, ""}
		};

		vertexBuffer->SetLayout(layout);

		vertexArray->AddVertexBuffer(vertexBuffer);

		unsigned int indexData[]
		{
			0, 1, 2
		};

		std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>(indexData, sizeof(unsigned int) * 3 );

		vertexArray->SetIndexBuffer(indexBuffer);


		while (m_Running)
		{
			RenderCommand::SetClearColor(0.2f, 0.5f, 0.8f, 1.0f);
			RenderCommand::Clear();

			for (Layer* layer : m_LayerStack)
			{
				layer->Tick();
			}

			Renderer::Submit(vertexArray);

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
