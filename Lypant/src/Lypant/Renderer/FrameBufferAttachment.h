#pragma once

#include <glm/glm.hpp>

namespace lypant
{
	class FrameBufferAttachment
	{
	public:
		inline uint32_t GetID() const { return m_RendererID; }
		inline int GetWidth() const { return m_Width; }
		inline int GetHeight() const { return m_Height; }
		inline glm::vec2 GetDimensions() const { return { m_Width, m_Height }; }
	protected:
		FrameBufferAttachment() = default;
		virtual ~FrameBufferAttachment() = default;
	protected:
		uint32_t m_RendererID;
		int m_Width;
		int m_Height;
	};
}
