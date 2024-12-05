#include <Lypant.h>

class ExampleLayer : public lypant::Layer
{
public:
	void Tick() override
	{

	}

	void OnImGuiRender() override
	{
		
		{
			ImGui::Begin("Example Layer");
			ImGui::End();
		}
		
	}

	void OnEvent(lypant::Event& event) override
	{

	}
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
