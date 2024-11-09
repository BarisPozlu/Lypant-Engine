#pragma once

#include "Application.h"

#ifdef LYPANT_PLATFORM_WINDOWS

int main(int argc, char** argv)
{
	lypant::Application* app = lypant::CreateApplication();
	app->Run();
	delete app;
}

#endif
