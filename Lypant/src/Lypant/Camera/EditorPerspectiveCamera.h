#pragma once

#include "PerspectiveCamera.h"

namespace lypant
{
	class EditorPerspectiveCamera : public PerspectiveCamera
	{
	public:
		EditorPerspectiveCamera(const glm::vec3& position, float fovy, float aspectRatio, float zNear, float zFar);
		void Tick(float deltaTime);
		void OnEvent(class Event& event);
	private:
		struct CameraParams
		{
			float Fovy;
			float ZNear;
			float ZFar;
		};
		CameraParams m_Params;
		float m_Speed;
	};
}
