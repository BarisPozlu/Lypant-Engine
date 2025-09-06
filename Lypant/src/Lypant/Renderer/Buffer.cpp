#include <lypch.h>
#include "Buffer.h"
#include "GraphicsContext.h"
#include "Vulkan/VulkanBuffer.h"

namespace lypant
{
	std::shared_ptr<Buffer> VertexBuffer::Create(void* data, uint32_t size)
	{
		BufferSpecification spec;
		spec.Type = BufferType::VertexBuffer;
		spec.Size = size;
		spec.IsDynamic = false;
		spec.UsageFlags = BufferUsageFlagsTransferDst | BufferUsageFlagsVertexBuffer | BufferUsageFlagsDeviceAddress;
		spec.MemoryFlags = BufferMemoryFlagsGPULocal;

		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return std::make_shared<VulkanBuffer>(data, spec); break;
		}

		LY_CORE_ASSERT(false, "Unknown Graphics API");
		return nullptr;
	}

	std::shared_ptr<Buffer> IndexBuffer::Create(void* data, uint32_t count)
	{
		BufferSpecification spec;
		spec.Type = BufferType::IndexBuffer;
		spec.Size = count * sizeof(uint32_t);
		spec.IsDynamic = false;
		spec.UsageFlags = BufferUsageFlagsTransferDst | BufferUsageFlagsIndexBuffer;
		spec.MemoryFlags = BufferMemoryFlagsGPULocal;

		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return std::make_shared<VulkanBuffer>(data, spec); break;
		}

		LY_CORE_ASSERT(false, "Unknown Graphics API");
		return nullptr;
	}

	std::shared_ptr<Buffer> UniformBuffer::Create(uint32_t size, const void* data, bool isDynamic)
	{
		BufferSpecification spec;
		spec.Type = BufferType::UniformBuffer;
		spec.Size = size;
		spec.IsDynamic = isDynamic;
		spec.UsageFlags = BufferUsageFlagsUniformBuffer;
		
		if (isDynamic)
		{
			spec.MemoryFlags = BufferMemoryFlagsHostVisible;
		}

		else
		{
			spec.MemoryFlags = BufferMemoryFlagsGPULocal;
			spec.UsageFlags |= BufferUsageFlagsTransferDst;
		}

		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return std::make_shared<VulkanBuffer>(data, spec); break;
		}

		LY_CORE_ASSERT(false, "Unknown Graphics API");
		return nullptr;
	}

	std::shared_ptr<Buffer> StorageBuffer::Create(uint32_t size, const void* data, bool isDynamic)
	{
		BufferSpecification spec;
		spec.Type = BufferType::StorageBuffer;
		spec.Size = size;
		spec.IsDynamic = isDynamic;
		spec.UsageFlags = BufferUsageFlagsStorageBuffer;

		if (isDynamic)
		{
			spec.MemoryFlags = BufferMemoryFlagsHostVisible;
		}

		else
		{
			spec.MemoryFlags = BufferMemoryFlagsGPULocal;
			spec.UsageFlags |= BufferUsageFlagsTransferDst;
		}

		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return std::make_shared<VulkanBuffer>(data, spec); break;
		}

		LY_CORE_ASSERT(false, "Unknown Graphics API");
		return nullptr;
	}

	std::shared_ptr<Buffer> StagingBuffer::Create(uint32_t size)
	{
		BufferSpecification spec;
		spec.Type = BufferType::StagingBuffer;
		spec.Size = size;
		spec.IsDynamic = false;
		spec.UsageFlags = BufferUsageFlagsTransferSrc;
		spec.MemoryFlags = BufferMemoryFlagsHostVisible;

		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return std::make_shared<VulkanBuffer>(nullptr, spec); break;
		}

		LY_CORE_ASSERT(false, "Unknown Graphics API");
		return nullptr;
	}
}
