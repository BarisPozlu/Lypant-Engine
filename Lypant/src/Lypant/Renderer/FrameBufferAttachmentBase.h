#pragma once

namespace lypant
{
	class ColorAttachment
	{
	public:
		inline uint32_t GetID() const { return m_RendererID; }
		inline int GetWidth() const { return m_Width; }
		inline int GetHeight() const { return m_Height; }

		virtual void Bind(uint32_t slot) const = 0;
	protected:
		ColorAttachment() = default;
		virtual ~ColorAttachment() = default;
	protected:
		uint32_t m_RendererID;
		int m_Width;
		int m_Height;
	};

	class DepthStencilAttachment
	{
	public:
		inline uint32_t GetID() const { return m_RendererID; }
		inline int GetWidth() const { return m_Width; }
		inline int GetHeight() const { return m_Height; }
	protected:
		DepthStencilAttachment() = default;
		virtual ~DepthStencilAttachment() = default;
	protected:
		uint32_t m_RendererID;
		int m_Width;
		int m_Height;
	};
}
