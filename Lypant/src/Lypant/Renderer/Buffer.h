#pragma once

#include "Shader.h"

namespace lypant
{
	struct BufferSubBlock
	{
		BufferSubBlock(ShaderDataType type, const std::string& name) 
			: Name(name), Type(type), Size(GetSizeFromShaderDataType(type)), Offset(0)
		{

		}

		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		uint32_t Offset;
	};

	class BufferLayout
	{
	public:
		BufferLayout() : m_Stride(0)
		{

		}

		BufferLayout(const std::initializer_list<BufferSubBlock>& blocks) : m_Blocks(blocks)
		{
			m_Stride = 0;

			for (BufferSubBlock& block : m_Blocks)
			{
				block.Offset = m_Stride;
				m_Stride += block.Size;
			}
		}

		inline const std::vector<BufferSubBlock>& GetBlocks() const { return m_Blocks; }
		inline uint32_t GetStride() const { return m_Stride; }
	private:
		std::vector<BufferSubBlock> m_Blocks;
		uint32_t m_Stride;
	};

	class VertexBuffer
	{
	public:
		VertexBuffer(const void* data, uint32_t size);
		~VertexBuffer();
		void Bind() const;
		void Unbind() const;

		inline void SetLayout(const BufferLayout& layout) { m_Layout = layout; }
		inline const BufferLayout& GetLayout() const { return m_Layout; }
	private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;
	};

	class IndexBuffer
	{
	public:
		IndexBuffer(uint32_t* data, uint32_t count);
		~IndexBuffer();
		void Bind() const;
		void Unbind() const;

		inline uint32_t GetCount() const { return m_Count; }
	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
	};

	class UniformBuffer
	{
	public:
		UniformBuffer(uint32_t size, const void* data);
		~UniformBuffer();
		
		void BindBase(uint32_t index) const;
		void BindRange(uint32_t index, uint32_t offset, uint32_t size) const;
		void BufferSubData(uint32_t offset, uint32_t size, const void* data) const;

		void Bind() const;
		void Unbind() const;
	private:
		uint32_t m_RendererID;
	};
}
