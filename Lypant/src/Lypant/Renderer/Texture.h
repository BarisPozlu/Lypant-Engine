#pragma once

#include "lypch.h"

namespace lypant
{
	class Texture2D
	{
	public:
		Texture2D(const std::string& path);
		~Texture2D();
		void Bind(uint32_t slot) const;

		inline int GetWidth() const { return m_Width; }
		inline int GetHeight() const { return m_Height; }
	private:
		uint32_t m_RendererID;
		int m_Width;
		int m_Height;
	};
}
