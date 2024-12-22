#include "lypch.h"
#include "Input.h"
#include <GLFW/glfw3.h>
#include "Lypant/Core/Application.h"

namespace lypant
{
	Input::FrameInputData* Input::s_Data = nullptr;

	bool Input::IsKeyPressed(int keycode)
	{
		return glfwGetKey(Application::Get().GetWindow().GetGLFWwindow(), keycode) == GLFW_PRESS;
	}

	bool Input::IsMouseButtonPressed(int button)
	{
		return glfwGetMouseButton(Application::Get().GetWindow().GetGLFWwindow(), button) == GLFW_PRESS;
	}

	glm::vec2 Input::GetMousePosition()
	{
		double xPos, yPos;
		glfwGetCursorPos(Application::Get().GetWindow().GetGLFWwindow(), &xPos, &yPos);

		return { (float)xPos, (float)yPos };
	}

	float Input::GetMouseXPosition()
	{
		double xPos;
		glfwGetCursorPos(Application::Get().GetWindow().GetGLFWwindow(), &xPos, nullptr);
		return xPos;
	}

	float Input::GetMouseYPosition()
	{
		double yPos;
		glfwGetCursorPos(Application::Get().GetWindow().GetGLFWwindow(), nullptr, &yPos);
		return yPos;
	}

	glm::vec2 Input::GetMouseOffset()
	{
		return s_Data->MouseOffset;
	}

	float Input::GetMouseXOffset()
	{
		return GetMouseOffset().x;
	}

	float Input::GetMouseYOffset()
	{
		return GetMouseOffset().y;
	}

	void Input::Init()
	{
		s_Data = new Input::FrameInputData();
		s_Data->LastFrameMousePosition = GetMousePosition();
		s_Data->MouseOffset = glm::vec2(0);
	}

	void Input::Shutdown()
	{
		delete s_Data;
	}

	void Input::Tick(float deltaTime)
	{
		glm::vec2 currentMousePosition = GetMousePosition();
		s_Data->MouseOffset = currentMousePosition - s_Data->LastFrameMousePosition;
		s_Data->LastFrameMousePosition = currentMousePosition;
	}
}
