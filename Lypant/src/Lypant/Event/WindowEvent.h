#pragma once

#include "Event.h"

namespace lypant
{
	class LYPANT_API WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(int width, int height) : 
			m_Width(width), m_Height(height) {}

		inline unsigned int GetWidth() const { return m_Width; }
		inline unsigned int GetHeight() const { return m_Height; }

		DEFINE_EVENT_CATEGORY(EventCategoryWindow)
		DEFINE_EVENT_TYPE(WindowResize)

	private:
		unsigned int m_Width, m_Height;
	};

	class LYPANT_API WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		DEFINE_EVENT_CATEGORY(EventCategoryWindow)
		DEFINE_EVENT_TYPE(WindowClose)
	};
}
