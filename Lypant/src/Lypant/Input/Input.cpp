#include "lypch.h"
#include "Input.h"
#include <GLFW/glfw3.h>
#include "Lypant/Core/Application.h"

namespace lypant
{
	bool Input::IsKeyPressed(int keycode)
	{
		return glfwGetKey(Application::Get().GetWindow().GetGLFWwindow(), keycode) == GLFW_PRESS;
	}

	bool Input::IsMouseButtonPressed(int button)
	{
		return glfwGetMouseButton(Application::Get().GetWindow().GetGLFWwindow(), button) == GLFW_PRESS;
	}

	float Input::GetMouseX()
	{
		double xPos;
		glfwGetCursorPos(Application::Get().GetWindow().GetGLFWwindow(), &xPos, nullptr);
		return xPos;
	}

	float Input::GetMouseY()
	{
		double yPos;
		glfwGetCursorPos(Application::Get().GetWindow().GetGLFWwindow(), nullptr, &yPos);
		return yPos;
	}

}
