#include <Lypant.h>

using namespace lypant;

class ExampleLayer : public Layer
{
public:
	ExampleLayer()
	{
		m_Camera = std::make_shared<EditorPerspectiveCamera>(glm::vec3(0.0f), glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

		m_PointLight = std::make_shared<PointLight>(glm::vec3(0.01f), glm::vec3(4.0, 0.2, 0.3), glm::vec3(0.75f), glm::vec3(-2.0f, 0.0f, -2.5f));
		m_PointLight2 = std::make_shared<PointLight>(glm::vec3(0.01f), glm::vec3(1.0f), glm::vec3(0.75f), glm::vec3(1.0f, 0.0f, -3.5f));
		m_DirectionalLight = std::make_shared<DirectionalLight>(glm::vec3(0.01f), glm::vec3(10.0f), glm::vec3(0.75f), glm::vec3(0.0f, -1.0f, 0.0f));
		m_SpotLight = std::make_shared<SpotLight>(glm::vec3(0.01f), glm::vec3(4.0), glm::vec3(0.75f), glm::vec3(0.0f, 1.5f, -2.0f), glm::vec3(0.2f, -0.5f, -1.0f));
		m_Lights.reserve(4);
		m_Lights.push_back(m_DirectionalLight);
		m_DirectionalLight->SetDirection(glm::vec3(0.3f, -0.5f, -0.5f));

		m_Lights.push_back(m_SpotLight);

		std::shared_ptr<Material> lightMaterial = std::make_shared<Material>("shaders/LightObject.glsl", glm::vec3(m_PointLight->Diffuse));

		m_LightMesh = std::make_shared<Mesh>(DefaultGeometry::Cube, lightMaterial);

		m_Weapon = std::make_shared<Model>("models/weapon/weapon1.glb");
		m_WeaponPosition = glm::vec3(1.0f, 0.0f, -4.0f);
		m_WeaponOrientation = glm::angleAxis(glm::radians(-90.0f), glm::vec3(0, 1, 0)) * glm::angleAxis(glm::radians(-90.0f), glm::vec3(1, 0, 0));

		m_Skybox = std::make_shared<Skybox>("textures/skybox/right.jpg");
	}

	virtual void Tick(float deltaTime) override
	{
		m_Camera->Tick(deltaTime);

		Renderer::SetAntiAliasing(m_AASetting);
		Renderer::SetExposure(m_Exposure);

		Renderer::BeginScene(m_Camera, m_Lights);

		if (std::find(m_Lights.begin(), m_Lights.end(), m_PointLight) != m_Lights.end())
		{
			m_LightMesh->GetMaterial()->UpdateColor(m_PointLight->Diffuse);
			Renderer::Submit(m_LightMesh, glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), (glm::vec3)m_PointLight->Position), glm::radians(0.0f), glm::vec3(0, 1, 0)), glm::vec3(0.3f)));
		}

		if (std::find(m_Lights.begin(), m_Lights.end(), m_PointLight2) != m_Lights.end())
		{
			m_LightMesh->GetMaterial()->UpdateColor(m_PointLight2->Diffuse);
			Renderer::Submit(m_LightMesh, glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), (glm::vec3)m_PointLight2->Position), glm::radians(0.0f), glm::vec3(0, 1, 0)), glm::vec3(0.3f)));
		}

		if (std::find(m_Lights.begin(), m_Lights.end(), m_SpotLight) != m_Lights.end())
		{
			m_LightMesh->GetMaterial()->UpdateColor(m_SpotLight->Diffuse);
			Renderer::Submit(m_LightMesh, glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), (glm::vec3)m_SpotLight->Position), glm::radians(0.0f), glm::vec3(0, 1, 0)), glm::vec3(0.3f)));
		}

		Renderer::Submit(m_Weapon, glm::translate(glm::mat4(1.0f), m_WeaponPosition) * glm::scale(glm::mat4_cast(m_WeaponOrientation), glm::vec3(0.02f)));

		Renderer::Submit(m_Skybox);

		Renderer::EndScene();
	}

	virtual void OnEvent(Event& event) override
	{
		m_Camera->OnEvent(event);

		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<KeyPressEvent>([this](KeyPressEvent event)
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

	virtual void OnImGuiRender() override
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

		ImGui::DragFloat3("Weapon position", (float*)&m_WeaponPosition, 0.05f, -20, 20);

		static const char* options[]
		{
			"None", "MSAA2X", "MSAA4X", "MSAA8X", "MSAA16X"
		};

		ImGui::Combo("Anti Aliasing", (int*)&m_AASetting, options, 5);

		ImGui::DragFloat("Exposure", &m_Exposure, 0.01f, 0.1f, 5.0f);

		ImGui::End();
	}

private:
	std::shared_ptr<Model> m_Weapon;
	glm::vec3 m_WeaponPosition;
	glm::quat m_WeaponOrientation;

	std::shared_ptr<Mesh> m_LightMesh;

	std::shared_ptr<Skybox> m_Skybox;

	std::shared_ptr<EditorPerspectiveCamera> m_Camera;

	std::shared_ptr<PointLight> m_PointLight;
	std::shared_ptr<PointLight> m_PointLight2;
	std::shared_ptr<DirectionalLight> m_DirectionalLight;
	std::shared_ptr<SpotLight> m_SpotLight;
	std::vector<std::shared_ptr<Light>> m_Lights;

	AntiAliasingSetting m_AASetting = AntiAliasingSetting::MSAA16X;
	float m_Exposure = 1.0f;
};

class SandboxApp : public Application
{
public:
	SandboxApp()
	{
		PushLayer(new ExampleLayer());
	}

	virtual ~SandboxApp()
	{

	}

};

Application* lypant::CreateApplication()
{
	return new SandboxApp();
}
