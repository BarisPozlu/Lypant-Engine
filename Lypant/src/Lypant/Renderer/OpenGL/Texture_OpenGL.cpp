#include "lypch.h"

#ifdef LYPANT_OPENGL

#include "Lypant/Renderer/Texture.h"
#include <glad/glad.h>
#include "stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/integer.hpp"

namespace lypant
{
	static GLenum GetOpenGLTextureWrappingOption(TextureWrappingOption wrappingOption)
	{
		switch (wrappingOption)
		{
			case TextureWrappingOption::Repeat: return GL_REPEAT;
			case TextureWrappingOption::Clamp:  return GL_CLAMP_TO_BORDER;
		}

		LY_CORE_ASSERT(false, "Invalid texture wrapping option");
		return -1;
	}

	Texture2D::Texture2D(const std::string& path, bool linearSpace, bool generateMipmap, bool floatingBuffer) : m_Path(path)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

		int channels;
		stbi_set_flip_vertically_on_load(1);

		void* buffer;

		GLenum type;

		if (floatingBuffer)
		{
			buffer = stbi_loadf(path.c_str(), &m_Width, &m_Height, &channels, 0);
			type = GL_FLOAT;
		}

		else
		{
			buffer = stbi_load(path.c_str(), &m_Width, &m_Height, &channels, 0);
			type = GL_UNSIGNED_BYTE;
		}

		LY_CORE_ASSERT(buffer, "Failed to load the image!");

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		GLenum internalFormat;
		GLenum dataFormat;

		if (channels == 1)
		{
			floatingBuffer ? internalFormat = GL_R16F : internalFormat = GL_R8;
			dataFormat = GL_RED;
		}

		else if (channels == 3)
		{
			floatingBuffer ? internalFormat = GL_RGB16F : linearSpace ? internalFormat = GL_RGB8 : internalFormat = GL_SRGB8;
			dataFormat = GL_RGB;
		}

		else if (channels == 4)
		{
			floatingBuffer ? internalFormat = GL_RGBA16F : linearSpace ? internalFormat = GL_RGBA8 : internalFormat = GL_SRGB8_ALPHA8;
			dataFormat = GL_RGBA;
		}

		LY_CORE_ASSERT(channels == 1 || channels == 3 || channels == 4, "Number of channels in the texture is not supported.");

		if (generateMipmap)
		{
			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			int levels = glm::floor(glm::log2(glm::max(m_Width, m_Height))) + 1;
			glTextureStorage2D(m_RendererID, levels, internalFormat, m_Width, m_Height);
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, type, buffer);
			glGenerateTextureMipmap(m_RendererID);
		}

		else
		{
			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, type, buffer);
		}

		stbi_image_free(buffer);
	}

	Texture2D::Texture2D(int width, int height, unsigned char* data, bool linearSpace, bool floatingBuffer, int channels, TextureWrappingOption wrappingOption, bool IsDepthTexture, float* borderColor)
	{
		m_Width = width;
		m_Height = height;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GetOpenGLTextureWrappingOption(wrappingOption));
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GetOpenGLTextureWrappingOption(wrappingOption));
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (wrappingOption == TextureWrappingOption::Clamp && borderColor)
		{
			glTextureParameterfv(m_RendererID, GL_TEXTURE_BORDER_COLOR, borderColor);
		}

		GLenum internalFormat;
		GLenum dataFormat;
		GLenum type;

		if (IsDepthTexture)
		{
			glTextureStorage2D(m_RendererID, 1, GL_DEPTH_COMPONENT32F, width, height);
			return;
		}

		if (channels == 4)
		{
			dataFormat = GL_RGBA;

			if (floatingBuffer)
			{
				type = GL_FLOAT;
				internalFormat = GL_RGBA16F;
			}

			else
			{
				type = GL_UNSIGNED_BYTE;
				linearSpace ? internalFormat = GL_RGBA16 : internalFormat = GL_SRGB8_ALPHA8;
			}
		}

		else if (channels == 3)
		{
			dataFormat = GL_RGB;

			if (floatingBuffer)
			{
				type = GL_FLOAT;
				internalFormat = GL_RGB16F;
			}

			else
			{
				type = GL_UNSIGNED_BYTE;
				linearSpace ? internalFormat = GL_RGB16 : internalFormat = GL_SRGB8;
			}
		}

		else if (channels == 2)
		{
			dataFormat = GL_RG;

			if (floatingBuffer)
			{
				type = GL_FLOAT;
				internalFormat = GL_RG16F;
			}

			else
			{
				type = GL_UNSIGNED_BYTE;
				internalFormat = GL_RG8;
			}
		}

		glTextureStorage2D(m_RendererID, 1, internalFormat, width, height);

		if (data)
		{	
			glTextureSubImage2D(m_RendererID, 0, 0, 0, width, height, dataFormat, type, data);
		}
	}

	Texture2D::~Texture2D()
	{
		glDeleteTextures(1, &m_RendererID);
		if (m_Path.size())
		{
			s_Cache.erase(m_Path);
		}
	}

	void Texture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}


	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////


	Texture2DMultiSample::Texture2DMultiSample(int width, int height, int samples, bool floatingBuffer)
	{
		m_Width = width;
		m_Height = height;

		glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_RendererID);

		if (floatingBuffer)
		{
			glTextureStorage2DMultisample(m_RendererID, samples, GL_RGBA16F, width, height, GL_TRUE);
		}

		else
		{
			glTextureStorage2DMultisample(m_RendererID, samples, GL_RGBA16, width, height, GL_TRUE);
		}
	}

	Texture2DMultiSample::~Texture2DMultiSample()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void Texture2DMultiSample::Bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_RendererID);
	}

	Texture2DArray::Texture2DArray(int width, int height, int depth, TextureWrappingOption wrappingOption, bool IsDepthTexture, float* borderColor)
	{
		m_Width = width;
		m_Height = height;

		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_RendererID);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GetOpenGLTextureWrappingOption(wrappingOption));
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GetOpenGLTextureWrappingOption(wrappingOption));
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (wrappingOption == TextureWrappingOption::Clamp && borderColor)
		{
			glTextureParameterfv(m_RendererID, GL_TEXTURE_BORDER_COLOR, borderColor);
		}

		if (IsDepthTexture)
		{
			glTextureStorage3D(m_RendererID, 1, GL_DEPTH_COMPONENT32F, width, height, depth);
		}

		else
		{
			glTextureStorage3D(m_RendererID, 1, GL_RGB8, width, height, depth);
		}

	}

	Texture2DArray::~Texture2DArray()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void Texture2DArray::Bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////


	Cubemap::Cubemap(const std::string& path)
	{
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		constexpr int numberOfFaces = 6;
		const char* fileNames[]
		{
			"right", "left", "top", "bottom", "front", "back"
		};
		int channels;

		std::string directory = path.substr(0, path.find_last_of('/') + 1);
		std::string extension = path.substr(path.find_last_of('.'));

		stbi_set_flip_vertically_on_load(0);

		for (int i = 0; i < numberOfFaces; i++)
		{
			unsigned char* buffer = stbi_load((directory + fileNames[i] + extension).c_str(), &m_Width, &m_Height, &channels, 0);
			LY_CORE_ASSERT(buffer, "Failed to load the image!");
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB8, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
			stbi_image_free(buffer);
		}
	}

	Cubemap::Cubemap(int width, int height, bool generateMipmap, bool floatingBuffer)
	{
		m_Width = width;
		m_Height = height;

		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		if (generateMipmap)
		{
			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}

		else
		{
			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}

		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		GLenum internalFormat;
		GLenum type;

		if (floatingBuffer)
		{
			internalFormat = GL_RGB16F;
			type = GL_FLOAT;
		}

		else
		{
			internalFormat = GL_SRGB8;
			type = GL_UNSIGNED_BYTE;
		}

		constexpr int numberOfFaces = 6;

		for (int i = 0; i < numberOfFaces; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, GL_RGB, type, nullptr);
		}

		if (generateMipmap)
		{
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		}
	}

	Cubemap::~Cubemap()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void Cubemap::Bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	}
}

#endif
