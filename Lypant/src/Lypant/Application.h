#pragma once

#include "Core.h"

namespace lypant
{
	class LYPANT_API Application
	{
	public:
		Application();
		virtual ~Application();

		virtual void Run();
	};

	Application* CreateApplication();
}
