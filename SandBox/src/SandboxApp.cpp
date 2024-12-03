#include <Lypant.h>

class ExampleLayer : public lypant::Layer
{
public:
	void Tick() override
	{

	}

	void OnEvent(lypant::Event& event) override
	{
		LY_INFO(event.GetName());
	}
};

class SandboxApp : public lypant::Application
{
public:
	SandboxApp()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new lypant::ImGuiLayer());
	}

	~SandboxApp()
	{

	}

};

lypant::Application* lypant::CreateApplication()
{
	return new SandboxApp();
}
