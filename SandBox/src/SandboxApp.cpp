#include <Lypant.h>
#include <glm/glm.hpp>

class ExampleLayer : public lypant::Layer
{
public:
	ExampleLayer()
	{
		std::shared_ptr<lypant::VertexArray> vertexArray = std::make_shared<lypant::VertexArray>();

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

		std::shared_ptr<lypant::VertexBuffer> vertexBuffer = std::make_shared<lypant::VertexBuffer>(vertexData, sizeof(vertexData));

		lypant::BufferLayout layout
		{
			{lypant::ShaderDataType::Float3, "a_Position"}, {lypant::ShaderDataType::Float3, "a_Normal"}, {lypant::ShaderDataType::Float2, "a_TexCoord"}
		};

		vertexBuffer->SetLayout(layout);
		vertexArray->AddVertexBuffer(vertexBuffer);

		// draw arrays are not supported, we have to use indices.
		unsigned int indexData[36];
		for (int i = 0; i < 36; i++)
		{
			indexData[i] = i;
		}

		std::shared_ptr<lypant::IndexBuffer> indexBuffer = std::make_shared<lypant::IndexBuffer>(indexData, 36);

		vertexArray->SetIndexBuffer(indexBuffer);

		m_Camera = std::make_shared<lypant::EditorPerspectiveCamera>(glm::vec3(-1.8f, 0.7f, 4.0f), glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
		
		m_PointLight = std::make_shared<lypant::PointLight>(glm::vec3(0.1f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(-2.0f, 0.0f, -2.5f));
		m_PointLight2 = std::make_shared<lypant::PointLight>(glm::vec3(0.1f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f, 0.0f, -3.5f));
		m_DirectionalLight = std::make_shared<lypant::DirectionalLight>(glm::vec3(0.1f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		m_SpotLight = std::make_shared<lypant::SpotLight>(glm::vec3(0.1f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(-5.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, -1.0f));
		m_Lights.reserve(4);
		//m_Lights.push_back(m_SpotLight);

		std::shared_ptr<lypant::Material> lightMaterial = std::make_shared<lypant::Material>("shaders/LightObject.glsl", glm::vec3(m_PointLight->Diffuse));
		std::shared_ptr<lypant::Material> groundMaterial = std::make_shared<lypant::Material>("shaders/Object.glsl", glm::vec3(0.3f, 0.3f, 0.3f));
		std::shared_ptr<lypant::Material> cubeMaterial = std::make_shared<lypant::Material>("shaders/TexturedObject.glsl", "textures/container2.png", "textures/container2_specular.png");

		m_LightMesh = std::make_shared<lypant::Mesh>(vertexArray, lightMaterial);
		m_GroundMesh = std::make_shared<lypant::Mesh>(vertexArray, groundMaterial);
		m_CubeMesh = std::make_shared<lypant::Mesh>(vertexArray, cubeMaterial);
	}

	void Tick(float deltaTime) override
	{
		lypant::RenderCommand::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		lypant::RenderCommand::Clear();

		m_Camera->Tick(deltaTime);

		lypant::Renderer::BeginScene(m_Camera, m_Lights);

		if (std::find(m_Lights.begin(), m_Lights.end(), m_PointLight) != m_Lights.end())
		{
			m_LightMesh->GetMaterial()->UpdateColor(m_PointLight->Diffuse);
			lypant::Renderer::Submit(m_LightMesh, glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), (glm::vec3)m_PointLight->Position), glm::radians(0.0f), glm::vec3(0, 1, 0)), glm::vec3(0.3f)));
		}

		if (std::find(m_Lights.begin(), m_Lights.end(), m_PointLight2) != m_Lights.end())
		{
			m_LightMesh->GetMaterial()->UpdateColor(m_PointLight2->Diffuse);
			lypant::Renderer::Submit(m_LightMesh, glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), (glm::vec3)m_PointLight2->Position), glm::radians(0.0f), glm::vec3(0, 1, 0)), glm::vec3(0.3f)));
		}

		if (std::find(m_Lights.begin(), m_Lights.end(), m_SpotLight) != m_Lights.end())
		{
			m_LightMesh->GetMaterial()->UpdateColor(m_SpotLight->Diffuse);
			lypant::Renderer::Submit(m_LightMesh, glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), (glm::vec3)m_SpotLight->Position), glm::radians(0.0f), glm::vec3(0, 1, 0)), glm::vec3(0.3f)));
		}

		lypant::Renderer::Submit(m_CubeMesh, glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)), glm::radians(0.0f), glm::vec3(0, 1, 0)));
		lypant::Renderer::Submit(m_CubeMesh, glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, -5.0f)), glm::radians(45.0f), glm::vec3(0, 1, 0)));
		lypant::Renderer::Submit(m_CubeMesh, glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -7.0f)), glm::radians(180.0f), glm::vec3(0, 1, 0)));
		
		lypant::Renderer::Submit(m_GroundMesh, glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-0.8f, -0.55f, -5.0f)), glm::vec3(10.0f, 0.1f, 11.0f)));

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
			ImGui::ColorEdit3("Light1 color", &m_PointLight->Diffuse[0]);
			m_PointLight->Ambient = m_PointLight->Diffuse * 0.1f;
		}
		
		if (std::find(m_Lights.begin(), m_Lights.end(), m_PointLight2) != m_Lights.end())
		{
			ImGui::DragFloat3("Light2 position", (float*)&m_PointLight2->Position, 0.05f, -20.0f, 20.0f);		
			ImGui::ColorEdit3("Light2 color", &m_PointLight2->Diffuse[0]);
			m_PointLight2->Ambient = m_PointLight2->Diffuse * 0.1f;
		}

		if (std::find(m_Lights.begin(), m_Lights.end(), m_SpotLight) != m_Lights.end())
		{
			ImGui::DragFloat3("SpotLight position", (float*)&m_SpotLight->Position, 0.05f, -20.0f, 20.0f);
			ImGui::ColorEdit3("SpotLight color", &m_SpotLight->Diffuse[0]);
			m_SpotLight->Ambient = m_SpotLight->Diffuse * 0.1f;
			ImGui::SliderFloat3("SpotLight Direction", (float*)&m_SpotLight->Direction, -1, 1);
			m_SpotLight->SetDirection(m_SpotLight->Direction);
		}
		
		if (std::find(m_Lights.begin(), m_Lights.end(), m_DirectionalLight) != m_Lights.end())
		{
			ImGui::SliderFloat3("Directional Light", (float*)&m_DirectionalLight->Direction, -1, 1);
			m_DirectionalLight->SetDirection(m_DirectionalLight->Direction);
		}

		ImGui::End();
	}

private:
	std::shared_ptr<lypant::Mesh> m_LightMesh;
	std::shared_ptr<lypant::Mesh> m_GroundMesh;
	std::shared_ptr<lypant::Mesh> m_CubeMesh;

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
