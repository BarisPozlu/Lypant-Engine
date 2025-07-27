#include <Lypant.h>

using namespace lypant;

class ExampleLayer : public Layer
{
public:
	ExampleLayer()
	{
	}

	virtual void Tick(float deltaTime) override
	{
		Renderer::BeginRendering();

		Renderer::ClearImage();

		Renderer::EndRendering();
	}

	virtual void OnEvent(Event& event) override
	{
	}

	virtual void OnImGuiRender() override
	{
	}
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
