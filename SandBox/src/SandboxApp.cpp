#include <Lypant.h>

class ExampleLayer : public lypant::Layer
{
public:
	GENERATE_LAYER_BODY(ExampleLayer);

	void Tick(float deltaTime) override
	{

	}

	void OnImGuiRender() override
	{
		
		{
			ImGui::Begin("Example Layer");
			ImGui::End();
		}
		
	}

	bool OnKeyPressEvent(lypant::KeyPressEvent& event)
	{
		return true;
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
