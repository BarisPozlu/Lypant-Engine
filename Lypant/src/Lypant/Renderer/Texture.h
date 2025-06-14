#pragma once

#include "lypch.h"
#include "FrameBufferAttachment.h"

namespace lypant
{
	enum class TextureWrappingOption
	{
		Repeat, Clamp
	};

	class Texture2D : public FrameBufferAttachment
	{
	public:
		inline static std::shared_ptr<Texture2D> Load(const std::string& path, bool linearSpace = true, bool generateMipmap = true, bool floatingBuffer = false, TextureWrappingOption wrappingOption = TextureWrappingOption::Repeat)
		{
			const auto it = s_Cache.find(path);
			if (it != s_Cache.end())
			{
				return std::shared_ptr<Texture2D>(it->second);
			}

			std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>(path, linearSpace, generateMipmap, floatingBuffer, wrappingOption);
			s_Cache[path] = std::weak_ptr<Texture2D>(texture);
			return texture;
		}
	public:
		Texture2D(const std::string& path, bool linearSpace, bool generateMipmap, bool floatingBuffer, TextureWrappingOption wrappingOption);
		Texture2D(int width, int height, unsigned char* data = nullptr, bool linearSpace = true, bool floatingBuffer = false, int channels = 3, TextureWrappingOption wrappingOption = TextureWrappingOption::Repeat, bool IsDepthTexture = false, float* borderColor = nullptr);
		virtual ~Texture2D();
		void Bind(uint32_t slot) const;
	private:
		inline static std::unordered_map<std::string, std::weak_ptr<Texture2D>> s_Cache;
	private:
		std::string m_Path;
	};

	class Texture2DMultiSample : public FrameBufferAttachment
	{
	public:
		Texture2DMultiSample(int width, int height, int samples, bool floatingBuffer = false);
		virtual ~Texture2DMultiSample();
		void Bind(uint32_t slot) const;
	};

	class Texture2DArray : public FrameBufferAttachment
	{
	public:
		Texture2DArray(int width, int height, int depth, TextureWrappingOption wrappingOption, bool IsDepthTexture, float* borderColor);
		virtual ~Texture2DArray();
		void Bind(uint32_t slot) const;
		inline int GetDepth() const { return m_Depth; }
	private:
		int m_Depth;
	};

	class Cubemap : public FrameBufferAttachment
	{
	public:
		// The path expected is the path of one of the textures. Textures should be named as "top", "bottom" etc and they should be in the same directory.
		Cubemap(const std::string& path);
		// Cubemap is not loaded from the optional string parameter, It is supposed to be given when creating from an equirectangular texture
		Cubemap(int width, int height, bool generateMipMap = false, bool floatingBuffer = false, const std::string& path = std::string());
		virtual ~Cubemap();
		void Bind(uint32_t slot) const;
		inline const std::string& GetPath() const { return m_Path; }
	private:
		std::string m_Path;
	};

	class CubemapArray : public FrameBufferAttachment
	{
	public:
		CubemapArray(int width, int height, int depth, bool IsDepthTexture);
		virtual ~CubemapArray();
		void Bind(uint32_t slot) const;
		inline int GetDepth() const { return m_Depth; }
	private:
		int m_Depth;
	};
}
