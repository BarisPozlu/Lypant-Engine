#pragma once

#include "lypch.h"

namespace lypant
{
	class Texture2D
	{
	public:
		inline static std::shared_ptr<Texture2D> Load(const std::string& path)
		{
			const auto it = s_Cache.find(path);
			if (it != s_Cache.end())
			{
				return std::shared_ptr<Texture2D>(it->second);
			}

			std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>(path);
			s_Cache[path] = std::weak_ptr<Texture2D>(texture);
			return texture;
		}
	public:
		Texture2D(const std::string& path);
		~Texture2D();
		void Bind(uint32_t slot) const;

		inline int GetWidth() const { return m_Width; }
		inline int GetHeight() const { return m_Height; }
	private:
		inline static std::unordered_map<std::string, std::weak_ptr<Texture2D>> s_Cache;
	private:
		uint32_t m_RendererID;
		std::string m_Path;
		int m_Width;
		int m_Height;
	};

	class Cubemap
	{
	public:
		// the path expected is the path of the right texture. Other textures should be named as "top", "bottom" etc and they should be in the same directory as the right texture.
		Cubemap(const std::string& path);
		~Cubemap();
		void Bind() const;

		inline int GetWidth() const { return m_Width; }
		inline int GetHeight() const { return m_Height; }
	private:
		uint32_t m_RendererID;
		int m_Width;
		int m_Height;
	};
}
