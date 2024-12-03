#include "lypch.h"
#include "ImGuiLayer.h"
#include "imgui.h"
#include "Platform/OpenGL/imgui_impl_opengl3.h"
#include "Lypant/Application.h"
#include "Lypant/Event/KeyEvent.h"
#include "Lypant/Event/MouseEvent.h"
#include "Lypant/Event/WindowEvent.h"

#include <glad/glad.h> // temp
#include <GLFW/glfw3.h> // temp

namespace lypant
{
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

		// TODO: Key mappings

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
		ImGuiIO& io = ImGui::GetIO();
		// rewrite this to check if the keys are pressed using LYPANT API.

		/*io.AddKeyEvent(ImGuiMod_Ctrl, (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS));
		io.AddKeyEvent(ImGuiMod_Shift, (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS));
		io.AddKeyEvent(ImGuiMod_Alt, (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS));
		io.AddKeyEvent(ImGuiMod_Super, (glfwGetKey(window, GLFW_KEY_LEFT_SUPER) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SUPER) == GLFW_PRESS));*/


		/* ImGuiKey key = LypantKeyToImGuiKey(LypantKey key); */
		//io.AddKeyEvent(key, true);

		return false;
	}

	bool ImGuiLayer::OnKeyReleaseEvent(KeyReleaseEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();

		/* ImGuiKey key = LypantKeyToImGuiKey(LypantKey key); */
		//io.AddKeyEvent(key, false);
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
		ImGuiIO& io = ImGui::GetIO();

		// rewrite this to check if the keys are pressed using LYPANT API.

		/*io.AddKeyEvent(ImGuiMod_Ctrl, (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS));
		io.AddKeyEvent(ImGuiMod_Shift, (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS));
		io.AddKeyEvent(ImGuiMod_Alt, (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS));
		io.AddKeyEvent(ImGuiMod_Super, (glfwGetKey(window, GLFW_KEY_LEFT_SUPER) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SUPER) == GLFW_PRESS));*/

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
}
