#include <Lypant.h>

class ExampleLayer : public lypant::Layer
{
public:
	void Tick() override
	{
		LY_TRACE("Layer");
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
	}

	~SandboxApp()
	{

	}

};

lypant::Application* lypant::CreateApplication()
{
	return new SandboxApp();
}
