#include <Lypant.h>

using namespace lypant;

class ExampleLayer : public Layer
{
public:
	ExampleLayer()
	{
		m_Scene = std::make_unique<Scene>();

		m_Weapon = m_Scene->LoadModel("models/weapon/weapon1.glb")[0];
		m_Weapon.GetComponent<TransformComponent>().Position = glm::vec3(1.3f, 1.0f, -7.5f);
		m_Weapon.GetComponent<TransformComponent>().Scale = glm::vec3(0.015f);
		m_Weapon.GetComponent<TransformComponent>().Rotation = glm::normalize(glm::angleAxis(glm::radians(-90.0f), glm::vec3(0, 1, 0)) * glm::angleAxis(glm::radians(-90.0f), glm::vec3(1, 0, 0)));

		//m_SkyLight = m_Scene->CreateEntity();
		//m_SkyLight.AddComponent<SkyLightComponent>("textures/example1.hdr", true);

		m_DirectionalLight = m_Scene->CreateEntity();
		m_DirectionalLightComponent = &m_DirectionalLight.AddComponent<DirectionalLightComponent>(glm::vec3(5.0f), glm::vec3(0.1f, -0.8f, 0.0f));

		m_Camera = std::make_shared<EditorPerspectiveCamera>(glm::vec3(0.0f), glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 200.0f);
	}

	virtual void Tick(float deltaTime) override
	{
		m_Camera->Tick(deltaTime);
		m_Scene->Tick(deltaTime, m_Camera);
	}

	virtual void OnEvent(Event& event) override
	{
		m_Camera->OnEvent(event);
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("hey");

		ImGui::SliderFloat3("Directional Light", (float*)&m_DirectionalLightComponent->Direction, -1, 1);
		m_DirectionalLightComponent->SetDirection(m_DirectionalLightComponent->Direction);

		ImGui::End();
	}
private:
	std::unique_ptr<Scene> m_Scene;
	Entity m_Weapon;
	Entity m_SkyLight;
	Entity m_DirectionalLight;
	DirectionalLightComponent* m_DirectionalLightComponent;
	std::shared_ptr<EditorPerspectiveCamera> m_Camera;
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
