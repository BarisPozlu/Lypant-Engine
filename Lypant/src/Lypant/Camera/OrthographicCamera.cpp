#include "lypch.h"
#include "OrthographicCamera.h"
#include "glm/gtc/matrix_transform.hpp"

namespace lypant
{
	OrthographicCamera::OrthographicCamera(const glm::vec3& position, float rotation, float left, float right, float bottom, float top)
		: m_Position(position), m_Rotation(rotation), m_ProjectionMatrix(glm::ortho(left, right, bottom, top))
	{
		m_ViewMatrix = glm::rotate(glm::mat4(1), -glm::radians(rotation), glm::vec3(0, 0, 1));
		m_ViewMatrix = glm::translate(m_ViewMatrix, -position);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}
