#pragma once

#include "lypch.h"
#include "FrameBufferAttachmentBase.h"

namespace lypant
{
	class Texture2D : public ColorAttachment
	{
	public:
		inline static std::shared_ptr<Texture2D> Load(const std::string& path, bool linearSpace = true, bool generateMipmap = true)
		{
			const auto it = s_Cache.find(path);
			if (it != s_Cache.end())
			{
				return std::shared_ptr<Texture2D>(it->second);
			}

			std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>(path, linearSpace, generateMipmap);
			s_Cache[path] = std::weak_ptr<Texture2D>(texture);
			return texture;
		}
	public:
		Texture2D(const std::string& path, bool linearSpace, bool generateMipmap);
		Texture2D(int width, int height, unsigned char* data = nullptr, bool linearSpace = true, bool floatingBuffer = false);
		~Texture2D();
		void Bind(uint32_t slot) const;
	private:
		inline static std::unordered_map<std::string, std::weak_ptr<Texture2D>> s_Cache;
	private:
		std::string m_Path;
	};

	class Texture2DMultiSample : public ColorAttachment
	{
	public:
		Texture2DMultiSample(int width, int height, int samples, bool floatingBuffer = false);
		~Texture2DMultiSample();
		void Bind(uint32_t slot) const;
	};

	class Cubemap
	{
	public:
		// The path expected is the path of one of the textures. Textures should be named as "top", "bottom" etc and they should be in the same directory.
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
