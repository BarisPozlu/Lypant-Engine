#pragma once

#include "Event.h"

namespace lypant
{
	class LYPANT_API MouseMoveEvent : public Event
	{
	public:
		MouseMoveEvent(float posX, float posY) :
			m_PosX(posX), m_PosY(posY) {}

		inline float GetX() const { return m_PosX; }
		inline float GetY() const { return m_PosY; }

		DEFINE_EVENT_CATEGORY(EventCategoryInput | EventCategoryMouse)
		DEFINE_EVENT_TYPE(MouseMove)

	private:
		float m_PosX, m_PosY;
	};

	class LYPANT_API MouseScrollEvent : public Event
	{
	public:
		MouseScrollEvent(float XOffset, float YOffset) :
			m_XOffset(XOffset), m_YOffset(YOffset)
		{
		}

		inline float GetXOffset() const { return m_XOffset; }
		inline float GetYOffset() const { return m_YOffset; }

		DEFINE_EVENT_CATEGORY(EventCategoryInput | EventCategoryMouse)
		DEFINE_EVENT_TYPE(MouseScroll)

	private:
		float m_XOffset, m_YOffset;
	};


	class LYPANT_API MouseButtonEvent : public Event
	{
	public:
		inline int GetButton() const { return m_Button; }

		DEFINE_EVENT_CATEGORY(EventCategoryInput | EventCategoryMouse | EventCategoryMouseButton)

	protected:
		MouseButtonEvent(int button) :
			m_Button(button) {}

	protected:
		int m_Button;
	};


	class LYPANT_API MouseButtonPressEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressEvent(int button) :
			MouseButtonEvent(button) {}

		DEFINE_EVENT_TYPE(MouseButtonPress)
	};

	class LYPANT_API MouseButtonReleaseEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleaseEvent(int button) :
			MouseButtonEvent(button) {}

		DEFINE_EVENT_TYPE(MouseButtonRelease)
	};
}
