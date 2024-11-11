#include <Lypant.h>

class SandboxApp : public lypant::Application
{
public:
	SandboxApp()
	{
		LY_INFO("App started.");
	}

	~SandboxApp()
	{

	}

};

lypant::Application* lypant::CreateApplication()
{
	return new SandboxApp();
}
