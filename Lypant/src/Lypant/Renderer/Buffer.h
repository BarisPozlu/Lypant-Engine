#pragma once

#include <memory>

namespace lypant
{
	class VertexBuffer
	{
	public:
		static std::shared_ptr<VertexBuffer> Create(void* data, uint32_t size);
		virtual ~VertexBuffer() = default;
	};

	class IndexBuffer
	{
	public:
		static std::shared_ptr<IndexBuffer> Create(void* data, uint32_t count);
		virtual ~IndexBuffer() = default;
	};
}
