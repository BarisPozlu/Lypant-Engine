#pragma once

namespace lypant
{
	class GraphicsContext
	{
	public:
		void Init(void* windowHandle);
		void SwapBuffers() const;
	private:
		void* m_WindowHandle;
	};
}
