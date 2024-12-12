#include <Lypant.h>

class ExampleLayer : public lypant::Layer
{
public:
	GENERATE_LAYER_BODY(ExampleLayer);

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

	bool OnKeyPressEvent(lypant::KeyPressEvent& event)
	{
		LY_INFO("Key pressed.");
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
