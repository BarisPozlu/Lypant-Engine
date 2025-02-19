#include "lypch.h"

#ifdef LYPANT_OPENGL

#include "Lypant/Renderer/Texture.h"
#include <glad/glad.h>
#include "stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/integer.hpp"

namespace lypant
{
	Texture2D::Texture2D(const std::string& path) : m_Path(path)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

		stbi_set_flip_vertically_on_load(1);
		int channels;
		unsigned char* buffer = stbi_load(path.c_str(), &m_Width, &m_Height, &channels, 0);
		LY_CORE_ASSERT(buffer, "Failed to load the image!");

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int levels = glm::floor(glm::log2(glm::max(m_Width, m_Height))) + 1;

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
	
		glTextureStorage2D(m_RendererID, levels, internalFormat, m_Width, m_Height);
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, buffer);
	
		glGenerateTextureMipmap(m_RendererID);

		stbi_image_free(buffer);
	}

	Texture2D::~Texture2D()
	{
		glDeleteTextures(1, &m_RendererID);
		s_Cache.erase(m_Path);
	}

	void Texture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}
}

#endif
