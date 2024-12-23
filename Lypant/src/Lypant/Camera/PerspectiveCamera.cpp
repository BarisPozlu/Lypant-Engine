#include "lypch.h"
#include "PerspectiveCamera.h"
#include "glm/gtc/matrix_transform.hpp"

namespace lypant
{
	PerspectiveCamera::PerspectiveCamera(const glm::vec3& position, float fovy, float aspectRatio, float zNear, float zFar)
		: m_Position(position), m_Orientation(glm::quatLookAt(glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)))
	{
		m_ViewMatrix = glm::mat4_cast(glm::conjugate(m_Orientation)) * glm::translate(glm::mat4(1.0f), -position);
		m_ProjectionMatrix = glm::perspective(fovy, aspectRatio, zNear, zFar);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void PerspectiveCamera::AddMovementInput(const glm::vec3& offset)
	{
		m_Position += offset;
		UpdateViewAndViewProjection();
	}

	void PerspectiveCamera::AddPitchInput(float offset)
	{
		m_Orientation = glm::normalize(glm::angleAxis(glm::radians(-offset), GetRight()) * m_Orientation);
		UpdateViewAndViewProjection();
	}

	void PerspectiveCamera::AddYawInput(float offset)
	{
		m_Orientation = glm::normalize(glm::angleAxis(glm::radians(-offset), glm::vec3(0, 1, 0)) * m_Orientation);
		UpdateViewAndViewProjection();
	}

	glm::vec3 PerspectiveCamera::GetRight() const
	{
		return glm::normalize(m_Orientation * glm::vec3(1, 0, 0));
	}

	glm::vec3 PerspectiveCamera::GetForward() const
	{
		return glm::normalize(m_Orientation * glm::vec3(0, 0, -1));
	}

	glm::vec3 PerspectiveCamera::GetUp() const
	{
		return glm::normalize(m_Orientation * glm::vec3(0, 1, 0));
	}

	void PerspectiveCamera::UpdateViewAndViewProjection()
	{
		m_ViewMatrix = glm::mat4_cast(glm::conjugate(m_Orientation)) * glm::translate(glm::mat4(1.0f), -m_Position);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void PerspectiveCamera::UpdateProjectionAndViewProjection(float fovy, float aspectRatio, float zNear, float zFar)
	{
		m_ProjectionMatrix = glm::perspective(fovy, aspectRatio, zNear, zFar);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}
