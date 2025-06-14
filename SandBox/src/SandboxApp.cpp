#include <Lypant.h>

using namespace lypant;

class TestScript : public BehaviorComponentBase
{
	virtual void BeginScene() override
	{
		LY_INFO("Test script BeginScene");
	}

	virtual void Tick(float deltaTime) override
	{
		if (Input::IsKeyPressed(LY_KEY_LEFT))
		{
			GetComponent<TransformComponent>().Position.x -= 5.0f * deltaTime;
		}

		if (Input::IsKeyPressed(LY_KEY_RIGHT))
		{
			GetComponent<TransformComponent>().Position.x += 5.0f * deltaTime;
		}

		if (Input::IsKeyPressed(LY_KEY_UP))
		{
			GetComponent<TransformComponent>().Position.z -= 5.0f * deltaTime;
		}

		if (Input::IsKeyPressed(LY_KEY_DOWN))
		{
			GetComponent<TransformComponent>().Position.z += 5.0f * deltaTime;
		}

		if (Input::IsKeyPressed(LY_KEY_N))
		{
			GetComponent<TransformComponent>().Position.y -= 5.0f * deltaTime;
		}

		if (Input::IsKeyPressed(LY_KEY_M))
		{
			GetComponent<TransformComponent>().Position.y += 5.0f * deltaTime;
		}
	}
};

class ExampleLayer : public Layer
{
public:
	ExampleLayer()
	{
		m_Camera = std::make_shared<EditorPerspectiveCamera>(glm::vec3(0.0f, 1.0f, 2.5f), glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 500.0f);
		m_Scene = std::make_shared<Scene>();
		
		m_PointLights.reserve(2);

		glm::vec3 colors[]{ glm::vec3(15.0, 25.0, 5.0), glm::vec3(15.0f, 30.0f, 25.0f)};

		for (int i = 0; i < 2; i++)
		{
			m_PointLights.push_back(m_Scene->CreateEntity());
			m_PointLights[i].GetComponent<TransformComponent>().Position = glm::vec3(-1.0f, 1.0f, -6.5 + i * 3.2f);
			m_PointLights[i].GetComponent<TransformComponent>().Scale = glm::vec3(0.5f);
			m_PointLights[i].AddComponent<MeshComponent>(DefaultGeometry::Cube, std::make_shared<Material>("shaders/FlatColor.glsl", colors[i]));
			m_PointLights[i].AddComponent<PointLightComponent>(colors[i]);
			m_PointLights[i].AddComponent<BehaviorComponent>().Bind<TestScript>();
		}

		m_DirectionalLightEntity = m_Scene->CreateEntity();
		m_DirectionalLightComponent = &m_DirectionalLightEntity.AddComponent<DirectionalLightComponent>(glm::vec3(5.0f), glm::vec3(0.1f, -0.8f, 0.0f));

		m_Sponza = m_Scene->LoadModel("models/sponza/Sponza.gltf", true, false);
		for (auto& entity : m_Sponza)
		{
			entity.GetComponent<TransformComponent>().Scale = glm::vec3(0.01f);
			entity.GetComponent<TransformComponent>().Rotation = glm::normalize(glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 1, 0)));
		}
		
		// this specific model is just one 1 mesh
		m_Weapon = m_Scene->LoadModel("models/weapon/weapon1.glb")[0];
		m_Weapon.GetComponent<TransformComponent>().Position = glm::vec3(1.3f, 1.0f, -7.5f);
		m_Weapon.GetComponent<TransformComponent>().Scale = glm::vec3(0.015f);
		m_Weapon.GetComponent<TransformComponent>().Rotation = glm::normalize(glm::angleAxis(glm::radians(-90.0f), glm::vec3(0, 1, 0)) * glm::angleAxis(glm::radians(-90.0f), glm::vec3(1, 0, 0)));

		m_SkyLight = m_Scene->CreateEntity();
		m_SkyLight.AddComponent<SkyLightComponent>("textures/skybox/example2.jpeg", true);

		m_Cubemap1 = util::CreateCubemapFromEquirectangularTexture("textures/skybox/example2.jpeg");
		m_Cubemap2 = util::CreateCubemapFromEquirectangularTexture("textures/skybox/example1.hdr");
		m_Cubemap3 = util::CreateCubemapFromEquirectangularTexture("textures/skybox/flamingo_pan_4k.hdr");
	}

	virtual void Tick(float deltaTime) override
	{
		m_FPS = 1.0 / deltaTime;

		Renderer::SetAntiAliasing(m_AASetting);
		Renderer::SetExposure(m_Exposure);
		Renderer::SetBloom(m_IsBloomEnabled);

		m_Camera->Tick(deltaTime);
		m_Scene->Tick(deltaTime, m_Camera);
	}

	virtual void OnEvent(Event& event) override
	{
		m_Camera->OnEvent(event);

		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<KeyPressEvent>([this](KeyPressEvent& event) 
			{
				if (event.GetKeyCode() == LY_KEY_C)
				{
					static int counter = 1;

					if (counter == 0)
					{
						m_SkyLight.GetComponent<SkyLightComponent>().Cubemap = m_Cubemap1;
						counter++;
					}

					else if (counter == 1)
					{
						m_SkyLight.GetComponent<SkyLightComponent>().Cubemap = m_Cubemap2;
						counter++;
					}

					else
					{
						m_SkyLight.GetComponent<SkyLightComponent>().Cubemap = m_Cubemap3;
						counter = 0;
					}

				}

				return false;
			});
		
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");

		ImGui::Text(("FPS: " + std::to_string(m_FPS)).c_str());

		ImGui::Text("Press C to change the environment map");
		ImGui::Text("Press the arrow keys to change the position of the point lights");

		static const char* options[]
		{
			"None", "MSAA2X", "MSAA4X", "MSAA8X", "MSAA16X"
		};

		ImGui::Combo("Anti Aliasing", (int*)&m_AASetting, options, 5);

		ImGui::DragFloat("Exposure", &m_Exposure, 0.01f, 0.1f, 5.0f);

		ImGui::Checkbox("Bloom", &m_IsBloomEnabled);
		ImGui::DragFloat("Ambient Strength", &m_SkyLight.GetComponent<SkyLightComponent>().Intensity, 0.01f, 0.0f, 1.0f);

		ImGui::SliderFloat3("Directional Light", (float*)&m_DirectionalLightComponent->Direction, -1, 1);
		m_DirectionalLightComponent->SetDirection(m_DirectionalLightComponent->Direction);

		ImGui::End();
	}

private:
	std::shared_ptr<EditorPerspectiveCamera> m_Camera;
	std::shared_ptr<Scene> m_Scene;

	Entity m_SkyLight;
	std::vector<Entity> m_PointLights;
	std::vector<Entity> m_Sponza;
	Entity m_Weapon;
	Entity m_DirectionalLightEntity;

	DirectionalLightComponent* m_DirectionalLightComponent;

	std::shared_ptr<Cubemap> m_Cubemap1;
	std::shared_ptr<Cubemap> m_Cubemap2;
	std::shared_ptr<Cubemap> m_Cubemap3;

	float m_FPS = 0;
	AntiAliasingSetting m_AASetting = AntiAliasingSetting::MSAA16X;
	float m_Exposure = 1.0f;
	bool m_IsBloomEnabled = true;
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
