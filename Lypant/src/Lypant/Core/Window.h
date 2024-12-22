#pragma once

#include "lypch.h"
#include "Core.h"
#include "Lypant/Event/Event.h"
#include "Lypant/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace lypant
{
	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Heigth;

		WindowProps(const std::string& title = "Lypant Engine",
			unsigned int width = 1280,
			unsigned int height = 720) : Title(title), Width(width), Heigth(height)
		{

		}

	};

	class LYPANT_API Window
	{
	public:
		Window(const WindowProps& props = WindowProps());
		~Window();

		void Tick();
		inline void SetEventCallback(std::function<void(Event&)> func) { m_Data.EventCallback = func; }

		void SetVSync(bool enabled);
		inline bool IsVSync() const { return m_Data.IsVSync; };

		inline GLFWwindow* GetGLFWwindow() const { return m_Window; }
		inline unsigned int GetWidth() const { return m_Data.Width; }
		inline unsigned int GetHeight() const { return m_Data.Height; };

	private:
		GLFWwindow* m_Window;
		GraphicsContext m_GraphicsContext;

		// this is to give a pointer to glfw.
		struct WindowData
		{
			std::string Title;
			std::function<void(Event&)> EventCallback;
			unsigned int Width;
			unsigned int Height;
			bool IsVSync;
		};

		WindowData m_Data;
	};
}
