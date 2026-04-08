#pragma once

#include <memory>

namespace lypant
{
	enum class BufferType
	{
		Unknown, VertexBuffer, IndexBuffer, UniformBuffer, StorageBuffer, IndirectBuffer, StagingBuffer
	};

	enum BufferUsageFlags
	{
		BufferUsageFlagsNone = 0,
		BufferUsageFlagsTransferSrc = 1,
		BufferUsageFlagsTransferDst = 2,
		BufferUsageFlagsVertexBuffer = 4,
		BufferUsageFlagsIndexBuffer = 8,
		BufferUsageFlagsUniformBuffer = 16,
		BufferUsageFlagsStorageBuffer = 32,
		BufferUsageFlagsIndirectBuffer = 64,
		BufferUsageFlagsDeviceAddress = 128
	};

	enum BufferMemoryFlags
	{
		BufferMemoryFlagsNone = 0,
		BufferMemoryFlagsGPULocal = 1,
		BufferMemoryFlagsHostVisible = 2
	};

	// Each buffer type will populate this and pass it to the graphics API implementation.
	struct BufferSpecification
	{
		BufferType Type = BufferType::Unknown;
		uint32_t Size = 0;
		int UsageFlags = BufferUsageFlagsNone;
		int MemoryFlags = BufferMemoryFlagsNone;
		bool IsDynamic = false;
	};

	// Base class for graphics API specific buffers.
	// Each buffer type when created, returns a buffer handle.
	class Buffer
	{
	public:
		Buffer() = default;
		virtual ~Buffer() = default;
		virtual void UploadData(const void* data, uint32_t size, uint32_t offset) = 0;
		static std::shared_ptr<Buffer> CreateVertexBuffer(void* data, uint32_t size);
		static std::shared_ptr<Buffer> CreateIndexBuffer(void* data, uint32_t count);
		static std::shared_ptr<Buffer> CreateUniformBuffer(uint32_t size, const void* data, bool dynamic = false);
		static std::shared_ptr<Buffer> CreateStorageBuffer(uint32_t size, const void* data, bool dynamic = false);
		static std::shared_ptr<Buffer> CreateIndirectBuffer(uint32_t size, const void* data, bool dynamic = false);
		static std::shared_ptr<Buffer> CreateStagingBuffer(uint32_t size);
	};
}
