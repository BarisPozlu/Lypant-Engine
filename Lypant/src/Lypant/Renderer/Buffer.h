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

	// maybe just have one uniform buffer per pass which is divided into sub buffers that correspond to a different frame in flight
	// when the size is given etc you just double it, when you want to upload data or whatever get the current frame and do some pointer math
	// the desciptor for unfiorm buffers is going to created within the passes that might use it or materials
	// we will also be using dynamic uniform buffers for offsets that update the uniform buffer every frame

	class UniformBuffer
	{
	public:
		static std::shared_ptr<UniformBuffer> Create(uint32_t size, const void* data, bool dynamic = false);
		virtual ~UniformBuffer() = default;
		virtual void UploadData(const void* data, uint32_t size, uint32_t offset) = 0;
	};
}
