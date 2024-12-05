#pragma once

#include "lypch.h"

namespace lypant
{
	class Event;

	class LYPANT_API Layer
	{
	public:
		Layer() {}
		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void Tick() {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}
	};
}
