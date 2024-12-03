#pragma once

#include "Lypant/Core.h"

namespace lypant
{
	class LYPANT_API Input
	{
	public:
		static bool IsKeyPressed(int keycode);
		static bool IsMouseButtonPressed(int button);
		//TODO: return the mouse position as a vector using glm.
		static float GetMouseX();
		static float GetMouseY();
	};
}
