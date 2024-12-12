#pragma once

#include "lypch.h"

namespace lypant
{
	class LYPANT_API Layer
	{
	public:
		Layer() {}
		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void Tick() {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(class Event& event) {}

		virtual bool OnKeyPressEvent(class KeyPressEvent& event) { return false; }
		virtual bool OnKeyReleaseEvent(class KeyReleaseEvent& event) { return false; }
		virtual bool OnMouseMoveEvent(class MouseMoveEvent& event) { return false; }
		virtual bool OnMouseScrollEvent(class MouseScrollEvent& event) { return false; }
		virtual bool OnMouseButtonPressEvent(class MouseButtonPressEvent& event) { return false; }
		virtual bool OnMouseButtonReleaseEvent(class MouseButtonReleaseEvent& event) { return false; }
	};
}

#define GENERATE_LAYER_BODY(layerName) void OnEvent(lypant::Event& event) override\
{\
	lypant::EventDispatcher dispatcher(event);\
	dispatcher.Dispatch<lypant::KeyPressEvent>(LY_BIND_EVENT_FUNC(layerName::OnKeyPressEvent));\
	dispatcher.Dispatch<lypant::KeyReleaseEvent>(LY_BIND_EVENT_FUNC(layerName::OnKeyReleaseEvent));\
	dispatcher.Dispatch<lypant::MouseMoveEvent>(LY_BIND_EVENT_FUNC(layerName::OnMouseMoveEvent));\
	dispatcher.Dispatch<lypant::MouseScrollEvent>(LY_BIND_EVENT_FUNC(layerName::OnMouseScrollEvent));\
	dispatcher.Dispatch<lypant::MouseButtonPressEvent>(LY_BIND_EVENT_FUNC(layerName::OnMouseButtonPressEvent));\
	dispatcher.Dispatch<lypant::MouseButtonReleaseEvent>(LY_BIND_EVENT_FUNC(layerName::OnMouseButtonReleaseEvent));\
}
