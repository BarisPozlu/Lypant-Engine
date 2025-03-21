#include "lypch.h"

#ifdef LYPANT_OPENGL

#include "Lypant/Renderer/Texture.h"
#include <glad/glad.h>
#include "stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/integer.hpp"

namespace lypant
{
	Texture2D::Texture2D(const std::string& path, bool generateMipmap) : m_Path(path)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

		int channels;
		stbi_set_flip_vertically_on_load(1);

		unsigned char* buffer = stbi_load(path.c_str(), &m_Width, &m_Height, &channels, 0);
		LY_CORE_ASSERT(buffer, "Failed to load the image!");

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		GLenum internalFormat = 0;
		GLenum dataFormat = 0;

		if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		else if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}

		LY_CORE_ASSERT(channels == 3 || channels == 4, "Number of channels in the texture is not supported.");

		if (generateMipmap)
		{
			int levels = glm::floor(glm::log2(glm::max(m_Width, m_Height))) + 1;
			glTextureStorage2D(m_RendererID, levels, internalFormat, m_Width, m_Height);
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, buffer);
			glGenerateTextureMipmap(m_RendererID);
		}

		else
		{
			glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, buffer);
		}

		stbi_image_free(buffer);
	}

	Texture2D::Texture2D(int width, int height)
	{
		m_Width = width;
		m_Height = height;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureStorage2D(m_RendererID, 1, GL_RGBA8, width, height);
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


	Texture2DMultiSample::Texture2DMultiSample(int width, int height, int samples)
	{
		m_Width = width;
		m_Height = height;

		glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_RendererID);
		glTextureStorage2DMultisample(m_RendererID, samples, GL_RGBA8, width, height, GL_TRUE);
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
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB8, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
			stbi_image_free(buffer);
		}
	}

	Cubemap::~Cubemap()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void Cubemap::Bind() const
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	}
}

#endif
