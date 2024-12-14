#include "lypch.h"

#ifdef LYPANT_OPENGL

#include "Lypant/Renderer/VertexArray.h"
#include <glad/glad.h>

namespace lypant
{
	static GLenum GetGLTypeFromShaderDataType(ShaderDataType type)
	{
		switch (type)
		{
			case lypant::ShaderDataType::None:		LY_CORE_ASSERT(false, "Shader data type is none!"); return 0;
			case lypant::ShaderDataType::Float:		return GL_FLOAT;
			case lypant::ShaderDataType::Float2:	return GL_FLOAT;
			case lypant::ShaderDataType::Float3:	return GL_FLOAT;
			case lypant::ShaderDataType::Float4:	return GL_FLOAT;
			case lypant::ShaderDataType::Mat3:		return GL_FLOAT;
			case lypant::ShaderDataType::Mat4:		return GL_FLOAT;
			case lypant::ShaderDataType::Int:		return GL_INT;
		}

		LY_CORE_ASSERT(false, "Invalid Shader data type!");
		return 0;
	}

	VertexArray::VertexArray() : m_VertexAttribPointer(0)
	{
		glCreateVertexArrays(1, &m_RendererID);
	}

	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void VertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void VertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		LY_CORE_ASSERT(vertexBuffer->GetLayout().GetStride(), "Tried to add a vertex buffer with no layout to a vertex array!");

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		const auto& blocks = vertexBuffer->GetLayout().GetBlocks();
		for (; m_VertexAttribPointer < blocks.size(); m_VertexAttribPointer++)
		{
			const auto& block = blocks[m_VertexAttribPointer];
			glEnableVertexAttribArray(m_VertexAttribPointer);
			glVertexAttribPointer(m_VertexAttribPointer, GetCountFromShaderDataType(block.Type),
				GetGLTypeFromShaderDataType(block.Type),
				GL_FALSE,
				vertexBuffer->GetLayout().GetStride(), (const void*)block.Offset);
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}
}

#endif
