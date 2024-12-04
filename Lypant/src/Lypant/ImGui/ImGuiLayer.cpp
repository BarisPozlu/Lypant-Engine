#include "lypch.h"
#include "ImGuiLayer.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "Lypant/Application.h"
#include "Lypant/Event/KeyEvent.h"
#include "Lypant/Event/MouseEvent.h"
#include "Lypant/Event/WindowEvent.h"
#include "Lypant/Input/Input.h"

#include <glad/glad.h> // temp
#include <GLFW/glfw3.h> // temp

namespace lypant
{
    // ImGui helpers
    static ImGuiKey LypantKeyToImGuiKey(int keycode);
    static void UpdateKeyModifiers();

	ImGuiLayer::ImGuiLayer()
	{

	}

	ImGuiLayer::~ImGuiLayer()
	{

	}

	void ImGuiLayer::OnAttach()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		
		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		ImGui_ImplOpenGL3_Init("#version 460");
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Tick()
	{
		ImGui_ImplOpenGL3_NewFrame();

		static ImGuiIO& io = ImGui::GetIO();
		static Application& application = Application::Get();

		io.DisplaySize = ImVec2(application.GetWindow().GetWidth(), application.GetWindow().GetHeight());

		float time = (float)glfwGetTime(); // temp
		io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1 / 60.0f);
		m_Time = time;

		ImGui::NewFrame();

		ImGui::ShowDemoWindow();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiLayer::OnEvent(Event& event)
	{
		EventDispatcher eventDispatcher(event);
		eventDispatcher.Dispatch<KeyPressEvent>(LY_BIND_EVENT_FUNC(ImGuiLayer::OnKeyPressEvent));
		eventDispatcher.Dispatch<KeyReleaseEvent>(LY_BIND_EVENT_FUNC(ImGuiLayer::OnKeyReleaseEvent));
		eventDispatcher.Dispatch<KeyTypeEvent>(LY_BIND_EVENT_FUNC(ImGuiLayer::OnKeyTypeEvent));
		eventDispatcher.Dispatch<MouseMoveEvent>(LY_BIND_EVENT_FUNC(ImGuiLayer::OnMouseMoveEvent));
		eventDispatcher.Dispatch<MouseScrollEvent>(LY_BIND_EVENT_FUNC(ImGuiLayer::OnMouseScrollEvent));
		eventDispatcher.Dispatch<MouseButtonPressEvent>(LY_BIND_EVENT_FUNC(ImGuiLayer::OnMouseButtonPressEvent));
		eventDispatcher.Dispatch<MouseButtonReleaseEvent>(LY_BIND_EVENT_FUNC(ImGuiLayer::OnMouseButtonReleaseEvent));
		eventDispatcher.Dispatch<WindowResizeEvent>(LY_BIND_EVENT_FUNC(ImGuiLayer::OnWindowResizeEvent));
	}

	// TODO: Implement returning true for handled events.

	bool ImGuiLayer::OnKeyPressEvent(KeyPressEvent& event)
	{
        UpdateKeyModifiers();

		ImGuiIO& io = ImGui::GetIO();
        io.AddKeyEvent(LypantKeyToImGuiKey(event.GetKeyCode()), true);
		return false;
	}

	bool ImGuiLayer::OnKeyReleaseEvent(KeyReleaseEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
        io.AddKeyEvent(LypantKeyToImGuiKey(event.GetKeyCode()), false);
		return false;
	}

	bool ImGuiLayer::OnKeyTypeEvent(KeyTypeEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddInputCharacter(event.GetCodepoint());
		return false;
	}

	bool ImGuiLayer::OnMouseMoveEvent(MouseMoveEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddMousePosEvent(event.GetX(), event.GetY());
		return false;
	}

	bool ImGuiLayer::OnMouseScrollEvent(MouseScrollEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddMouseWheelEvent(event.GetXOffset(), event.GetYOffset());
		return false;
	}

	bool ImGuiLayer::OnMouseButtonPressEvent(MouseButtonPressEvent& event)
	{
        UpdateKeyModifiers();

		ImGuiIO& io = ImGui::GetIO();
		io.AddMouseButtonEvent(event.GetButton(), true);
		return false;
	}

	bool ImGuiLayer::OnMouseButtonReleaseEvent(MouseButtonReleaseEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddMouseButtonEvent(event.GetButton(), false);
		return false;
	}

	bool ImGuiLayer::OnWindowResizeEvent(WindowResizeEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(event.GetWidth(), event.GetHeight());
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
 		glViewport(0, 0, event.GetWidth(), event.GetHeight());  // temp
		return false;
	}

    static void UpdateKeyModifiers()
    {
        ImGuiIO& io = ImGui::GetIO();

        io.AddKeyEvent(ImGuiMod_Ctrl, Input::IsKeyPressed(LY_KEY_LEFT_CONTROL) || Input::IsKeyPressed(LY_KEY_RIGHT_CONTROL));
        io.AddKeyEvent(ImGuiMod_Shift, Input::IsKeyPressed(LY_KEY_LEFT_SHIFT) || Input::IsKeyPressed(LY_KEY_RIGHT_SHIFT));
        io.AddKeyEvent(ImGuiMod_Alt, Input::IsKeyPressed(LY_KEY_LEFT_ALT) || Input::IsKeyPressed(LY_KEY_RIGHT_ALT));
        io.AddKeyEvent(ImGuiMod_Super, Input::IsKeyPressed(LY_KEY_LEFT_SUPER) || Input::IsKeyPressed(LY_KEY_RIGHT_SUPER));
    }

    static ImGuiKey LypantKeyToImGuiKey(int keycode)
    {
        switch (keycode)
        {
        case LY_KEY_TAB: return ImGuiKey_Tab;
        case LY_KEY_LEFT: return ImGuiKey_LeftArrow;
        case LY_KEY_RIGHT: return ImGuiKey_RightArrow;
        case LY_KEY_UP: return ImGuiKey_UpArrow;
        case LY_KEY_DOWN: return ImGuiKey_DownArrow;
        case LY_KEY_PAGE_UP: return ImGuiKey_PageUp;
        case LY_KEY_PAGE_DOWN: return ImGuiKey_PageDown;
        case LY_KEY_HOME: return ImGuiKey_Home;
        case LY_KEY_END: return ImGuiKey_End;
        case LY_KEY_INSERT: return ImGuiKey_Insert;
        case LY_KEY_DELETE: return ImGuiKey_Delete;
        case LY_KEY_BACKSPACE: return ImGuiKey_Backspace;
        case LY_KEY_SPACE: return ImGuiKey_Space;
        case LY_KEY_ENTER: return ImGuiKey_Enter;
        case LY_KEY_ESCAPE: return ImGuiKey_Escape;
        case LY_KEY_APOSTROPHE: return ImGuiKey_Apostrophe;
        case LY_KEY_COMMA: return ImGuiKey_Comma;
        case LY_KEY_MINUS: return ImGuiKey_Minus;
        case LY_KEY_PERIOD: return ImGuiKey_Period;
        case LY_KEY_SLASH: return ImGuiKey_Slash;
        case LY_KEY_SEMICOLON: return ImGuiKey_Semicolon;
        case LY_KEY_EQUAL: return ImGuiKey_Equal;
        case LY_KEY_LEFT_BRACKET: return ImGuiKey_LeftBracket;
        case LY_KEY_BACKSLASH: return ImGuiKey_Backslash;
        case LY_KEY_RIGHT_BRACKET: return ImGuiKey_RightBracket;
        case LY_KEY_GRAVE_ACCENT: return ImGuiKey_GraveAccent;
        case LY_KEY_CAPS_LOCK: return ImGuiKey_CapsLock;
        case LY_KEY_SCROLL_LOCK: return ImGuiKey_ScrollLock;
        case LY_KEY_NUM_LOCK: return ImGuiKey_NumLock;
        case LY_KEY_PRINT_SCREEN: return ImGuiKey_PrintScreen;
        case LY_KEY_PAUSE: return ImGuiKey_Pause;
        case LY_KEY_KP_0: return ImGuiKey_Keypad0;
        case LY_KEY_KP_1: return ImGuiKey_Keypad1;
        case LY_KEY_KP_2: return ImGuiKey_Keypad2;
        case LY_KEY_KP_3: return ImGuiKey_Keypad3;
        case LY_KEY_KP_4: return ImGuiKey_Keypad4;
        case LY_KEY_KP_5: return ImGuiKey_Keypad5;
        case LY_KEY_KP_6: return ImGuiKey_Keypad6;
        case LY_KEY_KP_7: return ImGuiKey_Keypad7;
        case LY_KEY_KP_8: return ImGuiKey_Keypad8;
        case LY_KEY_KP_9: return ImGuiKey_Keypad9;
        case LY_KEY_KP_DECIMAL: return ImGuiKey_KeypadDecimal;
        case LY_KEY_KP_DIVIDE: return ImGuiKey_KeypadDivide;
        case LY_KEY_KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
        case LY_KEY_KP_SUBTRACT: return ImGuiKey_KeypadSubtract;
        case LY_KEY_KP_ADD: return ImGuiKey_KeypadAdd;
        case LY_KEY_KP_ENTER: return ImGuiKey_KeypadEnter;
        case LY_KEY_KP_EQUAL: return ImGuiKey_KeypadEqual;
        case LY_KEY_LEFT_SHIFT: return ImGuiKey_LeftShift;
        case LY_KEY_LEFT_CONTROL: return ImGuiKey_LeftCtrl;
        case LY_KEY_LEFT_ALT: return ImGuiKey_LeftAlt;
        case LY_KEY_LEFT_SUPER: return ImGuiKey_LeftSuper;
        case LY_KEY_RIGHT_SHIFT: return ImGuiKey_RightShift;
        case LY_KEY_RIGHT_CONTROL: return ImGuiKey_RightCtrl;
        case LY_KEY_RIGHT_ALT: return ImGuiKey_RightAlt;
        case LY_KEY_RIGHT_SUPER: return ImGuiKey_RightSuper;
        case LY_KEY_MENU: return ImGuiKey_Menu;
        case LY_KEY_0: return ImGuiKey_0;
        case LY_KEY_1: return ImGuiKey_1;
        case LY_KEY_2: return ImGuiKey_2;
        case LY_KEY_3: return ImGuiKey_3;
        case LY_KEY_4: return ImGuiKey_4;
        case LY_KEY_5: return ImGuiKey_5;
        case LY_KEY_6: return ImGuiKey_6;
        case LY_KEY_7: return ImGuiKey_7;
        case LY_KEY_8: return ImGuiKey_8;
        case LY_KEY_9: return ImGuiKey_9;
        case LY_KEY_A: return ImGuiKey_A;
        case LY_KEY_B: return ImGuiKey_B;
        case LY_KEY_C: return ImGuiKey_C;
        case LY_KEY_D: return ImGuiKey_D;
        case LY_KEY_E: return ImGuiKey_E;
        case LY_KEY_F: return ImGuiKey_F;
        case LY_KEY_G: return ImGuiKey_G;
        case LY_KEY_H: return ImGuiKey_H;
        case LY_KEY_I: return ImGuiKey_I;
        case LY_KEY_J: return ImGuiKey_J;
        case LY_KEY_K: return ImGuiKey_K;
        case LY_KEY_L: return ImGuiKey_L;
        case LY_KEY_M: return ImGuiKey_M;
        case LY_KEY_N: return ImGuiKey_N;
        case LY_KEY_O: return ImGuiKey_O;
        case LY_KEY_P: return ImGuiKey_P;
        case LY_KEY_Q: return ImGuiKey_Q;
        case LY_KEY_R: return ImGuiKey_R;
        case LY_KEY_S: return ImGuiKey_S;
        case LY_KEY_T: return ImGuiKey_T;
        case LY_KEY_U: return ImGuiKey_U;
        case LY_KEY_V: return ImGuiKey_V;
        case LY_KEY_W: return ImGuiKey_W;
        case LY_KEY_X: return ImGuiKey_X;
        case LY_KEY_Y: return ImGuiKey_Y;
        case LY_KEY_Z: return ImGuiKey_Z;
        case LY_KEY_F1: return ImGuiKey_F1;
        case LY_KEY_F2: return ImGuiKey_F2;
        case LY_KEY_F3: return ImGuiKey_F3;
        case LY_KEY_F4: return ImGuiKey_F4;
        case LY_KEY_F5: return ImGuiKey_F5;
        case LY_KEY_F6: return ImGuiKey_F6;
        case LY_KEY_F7: return ImGuiKey_F7;
        case LY_KEY_F8: return ImGuiKey_F8;
        case LY_KEY_F9: return ImGuiKey_F9;
        case LY_KEY_F10: return ImGuiKey_F10;
        case LY_KEY_F11: return ImGuiKey_F11;
        case LY_KEY_F12: return ImGuiKey_F12;
        case LY_KEY_F13: return ImGuiKey_F13;
        case LY_KEY_F14: return ImGuiKey_F14;
        case LY_KEY_F15: return ImGuiKey_F15;
        case LY_KEY_F16: return ImGuiKey_F16;
        case LY_KEY_F17: return ImGuiKey_F17;
        case LY_KEY_F18: return ImGuiKey_F18;
        case LY_KEY_F19: return ImGuiKey_F19;
        case LY_KEY_F20: return ImGuiKey_F20;
        case LY_KEY_F21: return ImGuiKey_F21;
        case LY_KEY_F22: return ImGuiKey_F22;
        case LY_KEY_F23: return ImGuiKey_F23;
        case LY_KEY_F24: return ImGuiKey_F24;
        default: return ImGuiKey_None;
        }
    }
}
