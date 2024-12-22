#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

namespace lypant
{
	class PerspectiveCamera
	{
	public:
		PerspectiveCamera(const glm::vec3& position, float fovy, float aspectRatio, float zNear, float zFar);
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

		void AddMovementInput(const glm::vec3& offset);
		void AddPitchInput(float offset);
		void AddYawInput(float offset);

		glm::vec3 GetRight() const;
		glm::vec3 GetForward() const;
		glm::vec3 GetUp() const;

		void UpdateViewProjection();
	private:
		glm::vec3 m_Position;
		glm::quat m_Orientation;

		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewProjectionMatrix;
	};
}
