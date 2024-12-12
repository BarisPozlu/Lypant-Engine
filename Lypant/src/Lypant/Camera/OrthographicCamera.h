#pragma once

// TODO: change the math lib to be lypant lib.

#include "glm/glm.hpp"

namespace lypant
{
	class OrthographicCamera
	{
	public:
		OrthographicCamera(const glm::vec3& position, float rotation, float left, float right, float bottom, float top);
		
		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline void SetPosition(const glm::vec3& position) { m_Position = position; }

		inline float GetRotation() const { return m_Rotation; }
		inline void SetRotation(float rotation) { m_Rotation = rotation; }

		inline const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
	private:
		glm::vec3 m_Position;
		float m_Rotation;

		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewProjectionMatrix;
	};
}
