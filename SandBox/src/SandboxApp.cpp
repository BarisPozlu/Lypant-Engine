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
	}
};

class ExampleLayer : public Layer
{
public:
	ExampleLayer()
	{
		m_Camera = std::make_shared<EditorPerspectiveCamera>(glm::vec3(0.0f, 1.0f, 2.5f), glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 500.0f);

		std::shared_ptr<Material> lightMaterial = std::make_shared<Material>("shaders/FlatColor.glsl", glm::vec3(20.0, 0.0, 0.0));

		m_Skybox1 = std::make_shared<Skybox>("textures/skybox/flamingo_pan_4k.hdr", true);
		m_Skybox2 = std::make_shared<Skybox>("textures/skybox/example2.jpeg", true);
		m_Skybox3 = std::make_shared<Skybox>("textures/skybox/example1.hdr", true);

		std::shared_ptr<Material> sphereMaterial = std::make_shared<Material>("shaders/Model_PBR.glsl", "textures/light-gold/albedo.png", "textures/light-gold/roughness.png", "textures/light-gold/metallic.png", "textures/light-gold/normal.png", "textures/light-gold/ao.png");
		std::shared_ptr<Material> rustedIronMaterial = std::make_shared<Material>("shaders/Model_PBR.glsl", "textures/rusted-iron/albedo.png", "textures/rusted-iron/roughness.png", "textures/rusted-iron/metallic.png", "textures/rusted-iron/normal.png", "textures/rusted-iron/ao.png");

		m_Scene = std::make_shared<Scene>();

		m_GoldSphereEntity = m_Scene->CreateEntity();
		m_GoldSphereEntity.GetComponent<TransformComponent>().Position = glm::vec3(1.0, 1.0, -4.0);
		m_GoldSphereEntity.GetComponent<TransformComponent>().Scale = glm::vec3(0.3f);
		m_GoldSphereEntity.AddComponent<MeshComponent>(DefaultGeometry::Sphere, sphereMaterial);
		
		m_LightEntity = m_Scene->CreateEntity();
		m_LightEntity.GetComponent<TransformComponent>().Position = glm::vec3(1.0, 1.0, -1.5);
		m_LightEntity.GetComponent<TransformComponent>().Scale = glm::vec3(0.5f);
		m_LightEntity.AddComponent<MeshComponent>(DefaultGeometry::Cube, lightMaterial);
		//m_LightEntity.AddComponent<PointLightComponent>(glm::vec3(10.0, 0.0, 0.0));
		
		//m_LightEntity2 = m_Scene->CreateEntity();
		//m_LightEntity2.GetComponent<TransformComponent>().Scale = glm::vec3(0.3f);
		//m_LightEntity2.AddComponent<MeshComponent>(DefaultGeometry::Cube, std::make_shared<Material>("shaders/FlatColor.glsl", glm::vec3(5.0, 5.0, 10.0)));
		//m_LightEntity2.AddComponent<PointLightComponent>(glm::vec3(5.0, 5.0, 10.0));

		// these specific models are just one 1 mesh
		m_Weapon = m_Scene->LoadModel("models/weapon/weapon1.glb")[0];
		m_Weapon.GetComponent<TransformComponent>().Position = glm::vec3(0.0f, 3.0f, -1.5f);
		m_Weapon.GetComponent<TransformComponent>().Scale = glm::vec3(0.02f);
		m_Weapon.GetComponent<TransformComponent>().Rotation = glm::normalize(glm::angleAxis(glm::radians(-90.0f), glm::vec3(0, 1, 0)) * glm::angleAxis(glm::radians(-90.0f), glm::vec3(1, 0, 0)));
		
		//m_Helmet = m_Scene->LoadModel("models/helmet/DamagedHelmet.gltf")[0];
		//m_Helmet.GetComponent<TransformComponent>().Position = glm::vec3(2.0f, 0.0f, -2.0f);
		//m_Helmet.GetComponent<TransformComponent>().Rotation = glm::normalize(glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0)));

		m_Weapon.AddComponent<BehaviorComponent>().Bind<TestScript>();

		m_DirectionalLightEntity = m_Scene->CreateEntity();
		m_DirectionalLightEntity.AddComponent<DirectionalLightComponent>(glm::vec3(5.0f), glm::vec3(0.0f, -1.0f, -1.0f));

		m_Sponza = m_Scene->LoadModel("models/sponza/Sponza.gltf", true, false);
		for (auto& entity : m_Sponza)
		{
			entity.GetComponent<TransformComponent>().Scale = glm::vec3(0.01f);
			entity.GetComponent<TransformComponent>().Rotation = glm::normalize(glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 1, 0)));
		}

	}

	virtual void Tick(float deltaTime) override
	{
		m_Camera->Tick(deltaTime);

		Renderer::SetAntiAliasing(m_AASetting);
		Renderer::SetExposure(m_Exposure);
		Renderer::SetBloom(m_IsBloomEnabled);

		if (m_ExampleCounter == 0)
		{
			m_Scene->Tick(deltaTime, m_Camera, m_Skybox1);
		}

		else if (m_ExampleCounter == 1)
		{
			m_Scene->Tick(deltaTime, m_Camera, m_Skybox2);
		}

		else
		{
			m_Scene->Tick(deltaTime, m_Camera, m_Skybox3);
		}
	}

	virtual void OnEvent(Event& event) override
	{
		m_Camera->OnEvent(event);

		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<KeyPressEvent>([this](KeyPressEvent event)
			{
				if (event.GetKeyCode() == LY_KEY_C)
				{
					m_ExampleCounter == 2 ? m_ExampleCounter = 0 : m_ExampleCounter++;
				}

				return false;
			});
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Hey");
		
		ImGui::DragFloat3("Light entity position", (float*)&m_LightEntity.GetComponent<TransformComponent>().Position, 0.05f, -20.0f, 20.0f);
		//ImGui::ColorEdit3("Light entity color", (float*)&m_LightEntity.GetComponent<PointLightComponent>().Color);
		
		static const char* options[]
		{
			"None", "MSAA2X", "MSAA4X", "MSAA8X", "MSAA16X"
		};

		ImGui::Combo("Anti Aliasing", (int*)&m_AASetting, options, 5);

		ImGui::DragFloat("Exposure", &m_Exposure, 0.01f, 0.1f, 5.0f);

		auto& directionalLightComponent = m_DirectionalLightEntity.GetComponent<DirectionalLightComponent>();

		ImGui::SliderFloat3("Directional Light", (float*)&directionalLightComponent.Direction, -1, 1);
		directionalLightComponent.SetDirection(directionalLightComponent.Direction);

		ImGui::Checkbox("Bloom", &m_IsBloomEnabled);

		ImGui::End();
	}

private:
	std::shared_ptr<Scene> m_Scene;
	Entity m_Weapon;
	Entity m_Helmet;
	Entity m_GoldSphereEntity;
	Entity m_LightEntity;
	Entity m_LightEntity2;

	std::vector<Entity> m_Sponza;

	Entity m_DirectionalLightEntity;

	std::shared_ptr<Skybox> m_Skybox1;
	std::shared_ptr<Skybox> m_Skybox2;
	std::shared_ptr<Skybox> m_Skybox3;

	int m_ExampleCounter = 0;

	std::shared_ptr<EditorPerspectiveCamera> m_Camera;

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
