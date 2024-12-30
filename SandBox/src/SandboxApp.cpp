#include <Lypant.h>
#include <glm/glm.hpp>

class ExampleLayer : public lypant::Layer
{
public:
	ExampleLayer() : m_LightPosition(-2.0f, 0.0f, -2.5f), m_LightColor(1.0f, 1.0f, 1.0f)
	{
		m_LightObjectShader = std::make_shared<lypant::Shader>("shaders/LightObject.glsl");
		m_ObjectShader = std::make_shared<lypant::Shader>("shaders/Object.glsl");
		m_TexturedObjectShader = std::make_shared<lypant::Shader>("shaders/TexturedObject.glsl");

		m_ObjectTexture = std::make_shared<lypant::Texture2D>("textures/container2.png");
		m_ObjectTexture->Bind(0);

		m_VertexArray = std::make_shared<lypant::VertexArray>();

		float vertexData[]
		{
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f, -1.0f, 0.0f,  0.0f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f, -1.0f, 0.0f,  0.0f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f, 0.0f,  0.0f,  1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f
		};

		m_VertexBuffer = std::make_shared<lypant::VertexBuffer>(vertexData, sizeof(vertexData));

		lypant::BufferLayout layout
		{
			{lypant::ShaderDataType::Float3, "a_Position"}, {lypant::ShaderDataType::Float3, "a_Normal"}, {lypant::ShaderDataType::Float2, "a_TexCoord"}
		};

		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		// draw arrays are not supported, we have to use indices.
		unsigned int indexData[36];
		for (int i = 0; i < 36; i++)
		{
			indexData[i] = i;
		}

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

		// light object
		m_LightObjectShader->Bind();
		m_LightObjectShader->SetUniformVec3Float("u_LightColor", &m_LightColor[0]);
		lypant::Renderer::Submit(m_VertexArray, m_LightObjectShader, glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), m_LightPosition), glm::radians(0.0f), glm::vec3(0, 1, 0)), glm::vec3(0.3f)));

		//textured objects
		m_TexturedObjectShader->Bind();
		m_TexturedObjectShader->SetUniformVec3Float("u_LightPosition", &m_LightPosition[0]);
		m_TexturedObjectShader->SetUniformVec3Float("u_LightColor", &m_LightColor[0]);
		m_TexturedObjectShader->SetUniformVec3Float("u_ViewPosition", (float*) &m_Camera->GetPosition());
		m_TexturedObjectShader->SetUniformInt("u_TexSlot", 0);

		lypant::Renderer::Submit(m_VertexArray, m_TexturedObjectShader, glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)), glm::radians(0.0f), glm::vec3(0, 1, 0)));
		lypant::Renderer::Submit(m_VertexArray, m_TexturedObjectShader, glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, -5.0f)), glm::radians(45.0f), glm::vec3(0, 1, 0)));
		lypant::Renderer::Submit(m_VertexArray, m_TexturedObjectShader, glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -7.0f)), glm::radians(180.0f), glm::vec3(0, 1, 0)));

		//floor
		static float floorColor[]
		{
			0.3f, 0.3f, 0.3f
		};

		m_ObjectShader->Bind();
		m_ObjectShader->SetUniformVec3Float("u_LightPosition", &m_LightPosition[0]);
		m_ObjectShader->SetUniformVec3Float("u_LightColor", &m_LightColor[0]);
		m_ObjectShader->SetUniformVec3Float("u_ViewPosition", (float*)&m_Camera->GetPosition());
		m_ObjectShader->SetUniformVec3Float("u_ObjectColor", floorColor);
		lypant::Renderer::Submit(m_VertexArray, m_ObjectShader, glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-0.8f, -0.55f, -5.0f)), glm::vec3(10.0f, 0.1f, 11.0f)));

		lypant::Renderer::EndScene();
	}

	void OnEvent(lypant::Event& event) override
	{
		m_Camera->OnEvent(event);
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Hey");

		ImGui::DragFloat3("Light position", &m_LightPosition[0], 0.05f, -20.0f, 20.0f);
		
		ImGui::ColorEdit3("Light color", &m_LightColor[0]);

		ImGui::End();
	}

private:
	std::shared_ptr<lypant::Shader> m_LightObjectShader;
	std::shared_ptr<lypant::Shader> m_ObjectShader;
	std::shared_ptr<lypant::Shader> m_TexturedObjectShader;

	std::shared_ptr<lypant::Texture2D> m_ObjectTexture;

	std::shared_ptr<lypant::VertexArray> m_VertexArray;
	std::shared_ptr<lypant::VertexBuffer> m_VertexBuffer;
	std::shared_ptr<lypant::IndexBuffer> m_IndexBuffer;

	std::shared_ptr<lypant::EditorPerspectiveCamera> m_Camera;

	glm::vec3 m_LightPosition;
	glm::vec3 m_LightColor;
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
