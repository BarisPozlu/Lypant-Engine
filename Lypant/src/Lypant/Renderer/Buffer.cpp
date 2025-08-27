#include <lypch.h>
#include "Buffer.h"
#include "GraphicsContext.h"
#include "Vulkan/VulkanBuffer.h"

namespace lypant
{
	std::shared_ptr<VertexBuffer> VertexBuffer::Create(void* data, uint32_t size)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return std::make_shared<VulkanVertexBuffer>(data, size); break;
		}

		LY_CORE_ASSERT(false, "Unknown Graphics API");
		return nullptr;
	}

	std::shared_ptr<IndexBuffer> IndexBuffer::Create(void* data, uint32_t count)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return std::make_shared<VulkanIndexBuffer>(data, count); break;
		}

		LY_CORE_ASSERT(false, "Unknown Graphics API");
		return nullptr;
	}

	std::shared_ptr<UniformBuffer> UniformBuffer::Create(uint32_t size, const void* data, bool isDynamic)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return std::make_shared<VulkanUniformBuffer>(size, data, isDynamic); break;
		}

		LY_CORE_ASSERT(false, "Unknown Graphics API");
		return nullptr;
	}
}
