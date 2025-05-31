#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

namespace lypant
{
	class PerspectiveCamera
	{
	public:
		PerspectiveCamera(const glm::vec3& position, float fovy, float aspectRatio, float zNear, float zFar);

		void AddMovementInput(const glm::vec3& offset);
		void AddPitchInput(float offset);
		void AddYawInput(float offset);

		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline float GetFovY() const { return m_Fovy; }
		inline float GetAspectRatio() const { return m_AspectRatio; }
		inline float GetNearPlane() const { return m_ZNear; }
		inline float GetFarPlane() const { return m_ZFar; }

		inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		inline const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

		glm::vec3 GetRight() const;
		glm::vec3 GetForward() const;
		glm::vec3 GetUp() const;

		void UpdateViewAndViewProjection();
		void UpdateProjectionAndViewProjection(float fovy, float aspectRatio, float zNear, float zFar);
	private:
		glm::vec3 m_Position;
		glm::quat m_Orientation;
		float m_Fovy;
		float m_AspectRatio;
		float m_ZNear;
		float m_ZFar;

		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewProjectionMatrix;
	};
}
