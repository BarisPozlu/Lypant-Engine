#include "lypch.h"
#include "Application.h"
#include "Event/WindowEvent.h"
#include "Layer.h"
#include "Lypant/ImGui/ImGuiLayer.h"
#include "Lypant/Renderer/Renderer.h"
#include "Lypant/Renderer/Shader.h" // temp
#include "Lypant/Camera/OrthographicCamera.h" // temp
#include "glm/glm.hpp" // temp
#include <GLFW/glfw3.h> // temp
#include "Lypant/Renderer/Texture.h" // temp

namespace lypant
{
	Application* Application::m_Instance = nullptr;

	Application::Application() : m_Window(std::make_unique<Window>())
	{
		LY_CORE_ASSERT(!m_Instance, "Application already exists.");
		m_Instance = this;

		m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		//temp
		std::shared_ptr<Shader> shader = std::make_unique<Shader>("../Lypant/src/Lypant/Renderer/OpenGL/Shaders/VertexShader.shader", "../Lypant/src/Lypant/Renderer/OpenGL/Shaders/FragmentShader.shader");
		shader->Bind();

		std::shared_ptr<Texture2D> texture = std::make_unique<Texture2D>("../Lypant/src/Lypant/Renderer/example.png");
		texture->Bind(0);

		shader->SetUniformInt("u_TexSlot", 0);

		float color[]
		{
			0.8f, 0.2f, 0.3f, 1.0f
		};

		shader->SetUniformVec4Float("u_Color", color);

		std::shared_ptr<VertexArray> vertexArray = std::make_shared<VertexArray>();
		vertexArray->Bind();
		
		float vertexData[]
		{
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>(vertexData, sizeof(vertexData));

		BufferLayout layout
		{
			{ShaderDataType::Float3, "a_Position"}, {ShaderDataType::Float2, "a_TexCoord"}
		};

		vertexBuffer->SetLayout(layout);

		vertexArray->AddVertexBuffer(vertexBuffer);

		unsigned int indexData[]
		{
			0, 1, 2, 2, 3, 0
		};

		std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>(indexData, sizeof(indexData));

		vertexArray->SetIndexBuffer(indexBuffer);


		std::shared_ptr<OrthographicCamera> camera = std::make_shared<OrthographicCamera>(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), 0, -0.5, 0.5, -0.5, 0.5);

		while (m_Running)
		{
			float time = glfwGetTime();
			float deltaTime = time - m_LastFrameTime;
			m_LastFrameTime = time;

			RenderCommand::SetClearColor(0.2f, 0.5f, 0.8f, 1.0f);
			RenderCommand::Clear();

			for (Layer* layer : m_LayerStack)
			{
				layer->Tick(deltaTime);
			}

			Renderer::BeginScene(camera);

			Renderer::Submit(vertexArray, shader);

			Renderer::EndScene();

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
