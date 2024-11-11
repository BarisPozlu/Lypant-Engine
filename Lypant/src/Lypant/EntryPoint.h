#pragma once

#include "Application.h"
#include "Log.h" // for now to init log

#ifdef LYPANT_PLATFORM_WINDOWS

int main(int argc, char** argv)
{
	lypant::Log::Init();

	lypant::Application* app = lypant::CreateApplication();
	app->Run();
	delete app;
}

#endif
