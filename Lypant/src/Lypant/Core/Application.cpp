#include "lypch.h"
#include "Application.h"
#include "Lypant/Event/WindowEvent.h"
#include "Layer.h"
#include "Lypant/ImGui/ImGuiLayer.h"
#include "Lypant/Renderer/Renderer.h"
#include "Lypant/Renderer/Shader.h" // temp
#include "Lypant/Camera/PerspectiveCamera.h" // temp
#include "glm/glm.hpp" // temp
#include <GLFW/glfw3.h> // temp
#include "Lypant/Renderer/Texture.h" // temp
#include "Lypant/Input/Input.h" // temp

namespace lypant
{
	Application* Application::s_Instance = nullptr;

	Application::Application() : m_Window(std::make_unique<Window>())
	{
		LY_CORE_ASSERT(!s_Instance, "Application already exists.");
		s_Instance = this;

		m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		Input::Init();
	}

	Application::~Application()
	{
		Input::Shutdown();
	}

	void Application::Run()
	{
		//temp
		std::shared_ptr<Shader> shader = std::make_shared<Shader>("../Lypant/src/Lypant/Renderer/OpenGL/Shaders/BasicShader.glsl");
		shader->Bind();

		std::shared_ptr<VertexArray> vertexArray = std::make_shared<VertexArray>();
		vertexArray->Bind();
		
		float vertexData[]
		{
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f,  0.5f,
			 0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f
		};


		std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>(vertexData, sizeof(vertexData));

		BufferLayout layout
		{
			{ShaderDataType::Float3, "a_Position"}
		};

		vertexBuffer->SetLayout(layout);
		vertexArray->AddVertexBuffer(vertexBuffer);

		unsigned int indexData[]
		{
			0, 1, 2,
			1, 3, 4,
			5, 6, 3,
			7, 3, 6,
			2, 4, 7,
			0, 7, 6,
			0, 5, 1,
			1, 5, 3,
			5, 0, 6,
			7, 4, 3,
			2, 1, 4,
			0, 2, 7
		};
	
		std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>(indexData, 36);

		vertexArray->SetIndexBuffer(indexBuffer);

		std::shared_ptr<PerspectiveCamera> camera = std::make_shared<PerspectiveCamera>(glm::vec3(-1.8f, 0.7f, 4.0f), glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

		while (m_Running)
		{
			float time = glfwGetTime();
			float deltaTime = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				RenderCommand::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
				RenderCommand::Clear();

				Input::Tick(deltaTime);

				for (Layer* layer : m_LayerStack)
				{
					layer->Tick(deltaTime);
				}

				if (Input::IsKeyPressed(LY_KEY_W))
				{
					camera->AddMovementInput(camera->GetForward() * 3.0f * deltaTime);
				}

				if (Input::IsKeyPressed(LY_KEY_A))
				{
					camera->AddMovementInput(-camera->GetRight() * 3.0f * deltaTime);
				}
				
				if (Input::IsKeyPressed(LY_KEY_S))
				{
					camera->AddMovementInput(-camera->GetForward() * 3.0f * deltaTime);
				}

				if (Input::IsKeyPressed(LY_KEY_D))
				{
					camera->AddMovementInput(camera->GetRight() * 3.0f * deltaTime);
				}

				if (Input::IsKeyPressed(LY_KEY_Q))
				{
					camera->AddMovementInput(glm::vec3(0, 1, 0) * 3.0f * deltaTime);
				}

				if (Input::IsKeyPressed(LY_KEY_E))
				{
					camera->AddMovementInput(glm::vec3(0, -1, 0) * 3.0f * deltaTime);
				}

				if (Input::IsMouseButtonPressed(LY_MOUSE_BUTTON_2))
				{
					camera->AddYawInput(Input::GetMouseXOffset() * 0.08f);
					camera->AddPitchInput(Input::GetMouseYOffset() * 0.08f);
				}
				
				Renderer::BeginScene(camera);

				Renderer::Submit(vertexArray, shader, glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0, 1, 0)));

				Renderer::EndScene();
			}

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

		dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& resizeEvent)
			{
				if (resizeEvent.GetWidth() == 0 || resizeEvent.GetHeight() == 0)
				{
					m_Minimized = true;
					return false;
				}

				m_Minimized = false;
				Renderer::OnWindowResize(resizeEvent.GetWidth(), resizeEvent.GetHeight());
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
