#pragma once

#include "Event.h"

namespace lypant
{
	class LYPANT_API KeyTypeEvent : public Event
	{
	public:
		KeyTypeEvent(unsigned int codepoint) 
			: codepoint(codepoint) {}
		
		inline unsigned int GetCodepoint() const { return codepoint; }

		DEFINE_EVENT_CATEGORY(EventCategoryInput | EventCategoryKeyboard);
		DEFINE_EVENT_TYPE(KeyType);

	private:
		unsigned int codepoint;
	};

	class LYPANT_API KeyEvent : public Event
	{
	public:
		inline int GetKeyCode() const { return m_KeyCode; }

		DEFINE_EVENT_CATEGORY(EventCategoryInput | EventCategoryKeyboard)

	protected:
		KeyEvent(int keyCode)
			: m_KeyCode(keyCode) {}

	protected:
		int m_KeyCode;
	};

	class LYPANT_API KeyPressEvent : public KeyEvent
	{
	public:
		KeyPressEvent(int keycode, int repeatCount)
			: KeyEvent(keycode), m_RepeatCount(repeatCount) { }

		inline int GetRepeatCount() const { return m_RepeatCount; }

		DEFINE_EVENT_TYPE(KeyPress)
	private:
		int m_RepeatCount;
	};

	class LYPANT_API KeyReleaseEvent : public KeyEvent
	{
	public:
		KeyReleaseEvent(int keyCode) 
			: KeyEvent(keyCode) {}

		DEFINE_EVENT_TYPE(KeyRelease)
	};

}
