#pragma once

namespace lypant
{
	class GraphicsContext
	{
	public:
		void Init(void* windowHandle);
		void SwapBuffers();
	private:
		void* m_WindowHandle;
	};
}
