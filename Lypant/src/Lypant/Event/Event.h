#pragma once

#include "lypch.h"
#include "Lypant/Core.h"

namespace lypant
{
	enum class EventType
	{
		None,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMove,
		KeyPress, KeyRelease, KeyType,
		MouseButtonPress, MouseButtonRelease, MouseMove, MouseScroll
	};

	enum EventCategory
	{
		EventCategoryNone = 0,
		EventCategoryWindow = 1,
		EventCategoryInput = 2,
		EventCategoryKeyboard = 4,
		EventCategoryMouse = 8,
		EventCategoryMouseButton = 16
	};

	class LYPANT_API Event
	{
		friend class EventDispatcher;

	public:
		virtual EventType GetEventType() const = 0;
		virtual int GetEventCategoryFlags() const = 0;
		#ifdef LYPANT_DEBUG
		virtual const char* GetName() const = 0;
		#endif

		inline bool IsInEventCategory(EventCategory category) const { return GetEventCategoryFlags() & category; }
		inline bool IsHandled() { return m_Handled; }
	protected:
		bool m_Handled = false;
	};

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& event) 
			: m_Event(event) {}
		
		template<typename T>
		bool Dispatch(std::function<bool(T&)> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}

	private:
		Event& m_Event;
	};
}

#define DEFINE_EVENT_CATEGORY(category) inline virtual int GetEventCategoryFlags() const override { return category; }

#ifdef LYPANT_DEBUG 

	#define DEFINE_EVENT_TYPE(type) inline static EventType GetStaticType() { return EventType::type; }\
                                    inline virtual EventType GetEventType() const override { return GetStaticType(); }\
                                    inline virtual const char* GetName() const override { return #type; }
#else

	#define DEFINE_EVENT_TYPE(type) inline static EventType GetStaticType() { return EventType::type; }\
                                    inline virtual EventType GetEventType() const override { return GetStaticType(); }
#endif
