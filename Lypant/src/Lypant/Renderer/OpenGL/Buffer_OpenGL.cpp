#include "lypch.h"

#ifdef LYPANT_OPENGL

#include "Lypant/Renderer/Buffer.h"
#include "Lypant/Renderer/Texture.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace lypant
{
	VertexBuffer::VertexBuffer(const void* data, uint32_t size)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void VertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void VertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}


	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////


	IndexBuffer::IndexBuffer(uint32_t* data, uint32_t count) 
		: m_Count(count)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * m_Count, data, GL_STATIC_DRAW);
	}

	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void IndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void IndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}


	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////


	UniformBuffer::UniformBuffer(uint32_t size, const void* data)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW);
	}

	UniformBuffer::~UniformBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void UniformBuffer::BindBase(uint32_t index) const
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, index, m_RendererID);
	}

	void UniformBuffer::BindRange(uint32_t index, uint32_t offset, uint32_t size) const
	{
		glBindBufferRange(GL_UNIFORM_BUFFER, index, m_RendererID, offset, size);
	}

	void UniformBuffer::BufferSubData(uint32_t offset, uint32_t size, const void* data) const
	{
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
	}

	void UniformBuffer::Bind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
	}

	void UniformBuffer::Unbind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}


	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////


	RenderBuffer::RenderBuffer(int width, int height)
	{
		m_Width = width;
		m_Height = height;

		glCreateRenderbuffers(1, &m_RendererID);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RendererID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	}

	RenderBuffer::~RenderBuffer()
	{
		glDeleteRenderbuffers(1, &m_RendererID);
	}


	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////


	RenderBufferMultiSample::RenderBufferMultiSample(int width, int height, int samples)
	{
		m_Width = width;
		m_Height = height;

		glCreateRenderbuffers(1, &m_RendererID);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RendererID);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
	}

	RenderBufferMultiSample::~RenderBufferMultiSample()
	{
		glDeleteRenderbuffers(1, &m_RendererID);
	}


	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////


	FrameBuffer::FrameBuffer()
	{
		glCreateFramebuffers(1, &m_RendererID);
	}

	FrameBuffer::~FrameBuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
	}

	void FrameBuffer::BindDefaultFrameBuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	}

	void FrameBuffer::BindDraw() const
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_RendererID);
	}

	void FrameBuffer::BindRead() const
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_RendererID);
	}

	void FrameBuffer::SetColorBufferToRender(int value) const
	{
		Bind();

		if (value == -1)
		{
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}

		else
		{
			glDrawBuffer(GL_COLOR_ATTACHMENT0 + value);
			glReadBuffer(GL_COLOR_ATTACHMENT0 + value);
		}
	}

	void FrameBuffer::AttachColorBuffer(const std::shared_ptr<FrameBufferAttachment>& attachment, int level, int cubemapFace)
	{
		Bind();

		if (dynamic_cast<Texture2D*>(attachment.get()))
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, attachment->GetID(), level);
		}

		else if (dynamic_cast<Texture2DMultiSample*>(attachment.get()))
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, attachment->GetID(), level);
		}

		else if (dynamic_cast<Cubemap*>(attachment.get()))
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + cubemapFace, attachment->GetID(), level);
		}

		m_ColorAttachment = attachment;
	}

	void FrameBuffer::AttachDepthStencilBuffer(const std::shared_ptr<FrameBufferAttachment>& attachment)
	{
		Bind();

		if (dynamic_cast<RenderBuffer*>(attachment.get()) || dynamic_cast<RenderBufferMultiSample*>(attachment.get()))
		{
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, attachment->GetID());
		}

		else if (dynamic_cast<Texture2D*>(attachment.get()))
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, attachment->GetID(), 0);
		}

		else if (dynamic_cast<Texture2DArray*>(attachment.get()))
		{
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, attachment->GetID(), 0);
		}

		m_DepthStencilAttachment = attachment;
	}

	void FrameBuffer::BlitToFrameBuffer(FrameBuffer* otherBuffer, int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1) const
	{
		BindRead();
		otherBuffer->BindDraw();
		glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	void FrameBuffer::BlitToDefault(int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1) const
	{
		BindRead();
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
}

#endif 
