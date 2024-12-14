#pragma once

namespace lypant
{
	enum class ShaderDataType
	{
		None, Float, Float2, Float3, Float4, Mat3, Mat4, Int
	};

	inline uint32_t GetSizeFromShaderDataType(ShaderDataType type)
	{
		switch (type)
		{
			case lypant::ShaderDataType::None:		LY_CORE_ASSERT(false, "Shader data type is none!"); return 0;
			case lypant::ShaderDataType::Float:		return 4;
			case lypant::ShaderDataType::Float2:	return 4 * 2;
			case lypant::ShaderDataType::Float3:	return 4 * 3;
			case lypant::ShaderDataType::Float4:	return 4 * 4;
			case lypant::ShaderDataType::Mat3:		return 4 * 3 * 3;
			case lypant::ShaderDataType::Mat4:		return 4 * 4 * 4;
			case lypant::ShaderDataType::Int:		return 4;		
		}

		LY_CORE_ASSERT(false, "Invalid Shader data type!");
		return 0;
	}

	inline uint32_t GetCountFromShaderDataType(ShaderDataType type)
	{
		switch (type)
		{
			case lypant::ShaderDataType::None:		LY_CORE_ASSERT(false, "Shader data type is none!"); return 0;
			case lypant::ShaderDataType::Float:		return 1;
			case lypant::ShaderDataType::Float2:	return 2;
			case lypant::ShaderDataType::Float3:	return 3;
			case lypant::ShaderDataType::Float4:	return 4;
			case lypant::ShaderDataType::Mat3:		return 3 * 3;
			case lypant::ShaderDataType::Mat4:		return 4 * 4;
			case lypant::ShaderDataType::Int:		return 1;
		}

		LY_CORE_ASSERT(false, "Invalid Shader data type!");
		return 0;
	}

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
		VertexBuffer(float* data, uint32_t size);
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
}
