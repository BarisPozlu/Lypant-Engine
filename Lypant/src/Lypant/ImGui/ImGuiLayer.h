#pragma once

#include "Lypant/Layer.h"

namespace lypant
{
	class KeyPressEvent;
	class KeyReleaseEvent;
	class KeyTypeEvent;
	class MouseMoveEvent;
	class MouseScrollEvent;
	class MouseButtonPressEvent;
	class MouseButtonReleaseEvent;
	class WindowResizeEvent;

	class LYPANT_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		virtual ~ImGuiLayer();

		virtual void OnAttach();
		virtual void OnDetach();
		virtual void Tick();
		virtual void OnEvent(Event& event);
	private:
		bool OnKeyPressEvent(KeyPressEvent& event);
		bool OnKeyReleaseEvent(KeyReleaseEvent& event);
		bool OnKeyTypeEvent(KeyTypeEvent& event);
		bool OnMouseMoveEvent(MouseMoveEvent& event);
		bool OnMouseScrollEvent(MouseScrollEvent& event);
		bool OnMouseButtonPressEvent(MouseButtonPressEvent& event);
		bool OnMouseButtonReleaseEvent(MouseButtonReleaseEvent& event);
		bool OnWindowResizeEvent(WindowResizeEvent& event);
		/*ImGuiKey LypantKeyToImGuiKey(LypantKey key);*/ // implement.
	private:
		float m_Time = 0; // temp
	};
}
