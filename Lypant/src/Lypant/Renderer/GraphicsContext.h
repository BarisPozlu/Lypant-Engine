#pragma once

namespace lypant
{
	class GraphicsContext
	{
	public:
		void Hint();
		void Init(void* windowHandle);
		void SwapBuffers() const;
	private:
		void* m_WindowHandle;
	};
}
