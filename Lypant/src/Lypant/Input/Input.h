#pragma once

#include "Lypant/Core/Core.h"
#include "KeyCodes.h"
#include "MouseButtonCodes.h"
#include "glm/glm.hpp"

namespace lypant
{
	class LYPANT_API Input
	{
	public:
		friend class Application;

		static bool IsKeyPressed(int keycode);
		static bool IsMouseButtonPressed(int button);

		static glm::vec2 GetMousePosition();
		static float GetMouseXPosition();
		static float GetMouseYPosition();

		static glm::vec2 GetMouseOffset();
		static float GetMouseXOffset();
		static float GetMouseYOffset();
	private:
		static void Init();
		static void Shutdown();
		static void Tick(float deltaTime);
	private:
		struct FrameInputData
		{
			glm::vec2 LastFrameMousePosition;
			glm::vec2 MouseOffset;
		};

		static FrameInputData* s_Data;
	};
}
