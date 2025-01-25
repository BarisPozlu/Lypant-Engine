#include <Lypant.h>
#include <glm/glm.hpp>

class ExampleLayer : public lypant::Layer
{
public:
	ExampleLayer()
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
		
		m_PointLight = std::make_shared<lypant::PointLight>(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.1f), glm::vec3(1.0f), glm::vec3(0.5f), glm::vec3(-2.0f, 0.0f, -2.5f));
		m_PointLight2 = std::make_shared<lypant::PointLight>(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.1f), glm::vec3(1.0f), glm::vec3(0.5f), glm::vec3(1.0f, 0.0f, -3.5f));
		m_DirectionalLight = std::make_shared<lypant::DirectionalLight>(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.1f), glm::vec3(1.0f), glm::vec3(0.5f), glm::vec3(0.0f, -1.0f, 0.0f));
		m_SpotLight = std::make_shared<lypant::SpotLight>(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.1f), glm::vec3(1.0f), glm::vec3(0.5f), glm::vec3(-5.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		m_Lights.reserve(4);
	}

	void Tick(float deltaTime) override
	{
		lypant::RenderCommand::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		lypant::RenderCommand::Clear();

		m_Camera->Tick(deltaTime);

		lypant::Renderer::BeginScene(m_Camera, m_Lights);

		// light objects
		m_LightObjectShader->Bind();

		if (std::find(m_Lights.begin(), m_Lights.end(), m_PointLight) != m_Lights.end())
		{
			m_LightObjectShader->SetUniformVec3Float("u_LightColor", (float*)&m_PointLight->Color);
			lypant::Renderer::Submit(m_VertexArray, m_LightObjectShader, glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), (glm::vec3)m_PointLight->Position), glm::radians(0.0f), glm::vec3(0, 1, 0)), glm::vec3(0.3f)));
		}

		if (std::find(m_Lights.begin(), m_Lights.end(), m_PointLight2) != m_Lights.end())
		{
			m_LightObjectShader->SetUniformVec3Float("u_LightColor", (float*) &m_PointLight2->Color);
			lypant::Renderer::Submit(m_VertexArray, m_LightObjectShader, glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), (glm::vec3)m_PointLight2->Position), glm::radians(0.0f), glm::vec3(0, 1, 0)), glm::vec3(0.3f)));
		}

		//textured objects
		m_TexturedObjectShader->Bind();
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
		m_ObjectShader->SetUniformVec3Float("u_ViewPosition", (float*)&m_Camera->GetPosition());
		m_ObjectShader->SetUniformVec3Float("u_ObjectColor", floorColor);
		
		lypant::Renderer::Submit(m_VertexArray, m_ObjectShader, glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-0.8f, -0.55f, -5.0f)), glm::vec3(10.0f, 0.1f, 11.0f)));

		lypant::Renderer::EndScene();
	}

	void OnEvent(lypant::Event& event) override
	{
		m_Camera->OnEvent(event);

		lypant::EventDispatcher dispatcher(event);

		dispatcher.Dispatch<lypant::KeyPressEvent>([this](lypant::KeyPressEvent event)
			{
				if (event.GetKeyCode() == LY_KEY_1)
				{
					if (std::find(m_Lights.begin(), m_Lights.end(), m_PointLight) != m_Lights.end())
					{
						m_Lights.erase(std::find(m_Lights.begin(), m_Lights.end(), m_PointLight));
					}

					else
					{
						m_Lights.emplace_back(m_PointLight);
					}
				}

				else if (event.GetKeyCode() == LY_KEY_2)
				{
					
					if (std::find(m_Lights.begin(), m_Lights.end(), m_PointLight2) != m_Lights.end())
					{
						m_Lights.erase(std::find(m_Lights.begin(), m_Lights.end(), m_PointLight2));
					}

					else
					{
						m_Lights.emplace_back(m_PointLight2);
					}
				}

				else if (event.GetKeyCode() == LY_KEY_3)
				{
					if (std::find(m_Lights.begin(), m_Lights.end(), m_DirectionalLight) != m_Lights.end())
					{
						m_Lights.erase(std::find(m_Lights.begin(), m_Lights.end(), m_DirectionalLight));
					}

					else
					{
						m_Lights.emplace_back(m_DirectionalLight);
					}
				}

				return false;
			});
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Hey");

		if (std::find(m_Lights.begin(), m_Lights.end(), m_PointLight) != m_Lights.end())
		{
			ImGui::DragFloat3("Light1 position", (float*)&m_PointLight->Position, 0.05f, -20.0f, 20.0f);
			ImGui::ColorEdit3("Light1 color", &m_PointLight->Color[0]);
		}
		
		if (std::find(m_Lights.begin(), m_Lights.end(), m_PointLight2) != m_Lights.end())
		{
			ImGui::DragFloat3("Light2 position", (float*) &m_PointLight2->Position, 0.05f, -20.0f, 20.0f);		
			ImGui::ColorEdit3("Light2 color", &m_PointLight2->Color[0]);
		}
		
		if (std::find(m_Lights.begin(), m_Lights.end(), m_DirectionalLight) != m_Lights.end())
		{
			ImGui::SliderFloat3("Directional Light", (float*)&m_DirectionalLight->Direction, -1, 1);
			m_DirectionalLight->SetDirection(m_DirectionalLight->Direction);
		}

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

	std::shared_ptr<lypant::PointLight> m_PointLight;
	std::shared_ptr<lypant::PointLight> m_PointLight2;
	std::shared_ptr<lypant::DirectionalLight> m_DirectionalLight;
	std::shared_ptr<lypant::SpotLight> m_SpotLight;

	std::vector<std::shared_ptr<lypant::Light>> m_Lights;
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
