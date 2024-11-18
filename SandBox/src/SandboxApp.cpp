#include <Lypant.h>

class SandboxApp : public lypant::Application
{
public:
	SandboxApp()
	{

	}

	~SandboxApp()
	{

	}

};

lypant::Application* lypant::CreateApplication()
{
	return new SandboxApp();
}
