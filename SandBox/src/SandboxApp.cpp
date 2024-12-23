#include <Lypant.h>
#include <glm/glm.hpp>

class ExampleLayer : public lypant::Layer
{
public:
	ExampleLayer()
	{
		m_Shader = std::make_shared<lypant::Shader>("../Lypant/src/Lypant/Renderer/OpenGL/Shaders/BasicShader.glsl");
		m_Shader->Bind();

		m_VertexArray = std::make_shared<lypant::VertexArray>();
		m_VertexArray->Bind();

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

		m_VertexBuffer = std::make_shared<lypant::VertexBuffer>(vertexData, sizeof(vertexData));

		lypant::BufferLayout layout
		{
			{lypant::ShaderDataType::Float3, "a_Position"}
		};

		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

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

		m_IndexBuffer = std::make_shared<lypant::IndexBuffer>(indexData, 36);

		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		m_Camera = std::make_shared<lypant::EditorPerspectiveCamera>(glm::vec3(-1.8f, 0.7f, 4.0f), glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
	}

	void Tick(float deltaTime) override
	{
		lypant::RenderCommand::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		lypant::RenderCommand::Clear();

		m_Camera->Tick(deltaTime);

		lypant::Renderer::BeginScene(m_Camera);

		lypant::Renderer::Submit(m_VertexArray, m_Shader, glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0, 1, 0)));

		lypant::Renderer::EndScene();
	}

	void OnEvent(lypant::Event& event) override
	{
		m_Camera->OnEvent(event);
	}

private:
	std::shared_ptr<lypant::Shader> m_Shader;
	std::shared_ptr<lypant::VertexArray> m_VertexArray;
	std::shared_ptr<lypant::VertexBuffer> m_VertexBuffer;
	std::shared_ptr<lypant::IndexBuffer> m_IndexBuffer;
	std::shared_ptr<lypant::EditorPerspectiveCamera> m_Camera;
};

class SandboxApp : public lypant::Application
{
public:
	SandboxApp()
	{
		PushLayer(new ExampleLayer());
	}

	~SandboxApp()
	{

	}

};

lypant::Application* lypant::CreateApplication()
{
	return new SandboxApp();
}
