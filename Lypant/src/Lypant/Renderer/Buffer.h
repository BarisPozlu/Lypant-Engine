#pragma once

namespace lypant
{
	class VertexBuffer
	{
	public:
		VertexBuffer(float* data, uint32_t size);
		~VertexBuffer();
		void Bind() const;
		void Unbind() const;
	private:
		uint32_t m_RendererID;
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
