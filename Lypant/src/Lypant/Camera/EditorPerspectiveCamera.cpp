#include "lypch.h"
#include "EditorPerspectiveCamera.h"
#include "Lypant/Input/Input.h"
#include "Lypant/Event/MouseEvent.h"
#include "Lypant/Event/WindowEvent.h"

namespace lypant
{
	EditorPerspectiveCamera::EditorPerspectiveCamera(const glm::vec3& position, float fovy, float aspectRatio, float zNear, float zFar)
		: PerspectiveCamera(position, fovy, aspectRatio, zNear, zFar), m_Speed(4.5f)
	{
		m_Params.Fovy= fovy;
		m_Params.ZNear = zNear;
		m_Params.ZFar = zFar;
	}

	void EditorPerspectiveCamera::Tick(float deltaTime)
	{
		glm::vec3 velocityDirection(0);

		if (Input::IsKeyPressed(LY_KEY_W))
		{
			velocityDirection += GetForward();
		}

		if (Input::IsKeyPressed(LY_KEY_A))
		{
			velocityDirection -= GetRight();
		}

		if (Input::IsKeyPressed(LY_KEY_S))
		{
			velocityDirection -= GetForward();
		}

		if (Input::IsKeyPressed(LY_KEY_D))
		{
			velocityDirection += GetRight();
		}

		if (Input::IsKeyPressed(LY_KEY_Q))
		{
			velocityDirection.y += 1;
		}

		if (Input::IsKeyPressed(LY_KEY_E))
		{
			velocityDirection.y -= 1;
		}

		if (velocityDirection != glm::vec3(0))
		{
			AddMovementInput(glm::normalize(velocityDirection) * m_Speed * deltaTime);
		}

		if (Input::IsMouseButtonPressed(LY_MOUSE_BUTTON_2))
		{
			AddYawInput(Input::GetMouseXOffset() * 0.08f);
			AddPitchInput(Input::GetMouseYOffset() * 0.08f);
		}
	}

	void EditorPerspectiveCamera::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<MouseScrollEvent>([this](MouseScrollEvent& event)
			{
				if (Input::IsMouseButtonPressed(LY_MOUSE_BUTTON_2))
				{
					m_Speed += event.GetYOffset();
					m_Speed = m_Speed < 0 ? 0 : m_Speed;
				}
				return false;
			});

		dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& event)
			{
				UpdateProjectionAndViewProjection(m_Params.Fovy, (float)event.GetWidth() / (float)event.GetHeight(), m_Params.ZNear, m_Params.ZFar);
				return false;
			});
	}
}