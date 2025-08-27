#include <lypch.h>
#include "VulkanBuffer.h"
#include "VulkanGraphicsContext.h"
#include "VulkanCommandBuffer.h"
// NOTE: this is required to get the current frame index and frames in flight for uniform buffers
#include <Lypant/Renderer/Renderer.h>
#include "VulkanRenderCommandBuffer.h"

namespace lypant
{
	VulkanVertexBuffer::VulkanVertexBuffer(void* data, uint32_t size)
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
		bufferInfo.size = size;

		VmaAllocationCreateInfo allocationInfo{};
		allocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		vmaCreateBuffer(graphicsContext.GetAllocator(), &bufferInfo, &allocationInfo, &m_Buffer, &m_Allocation, &m_AllocationInfo);

		VulkanStagingBuffer stagingBuffer(size);

		memcpy(stagingBuffer.GetMappedMemory(), data, size);

		{
			VkBufferCopy copy{};
			copy.size = size;

			VulkanImmediateCommandScope scope;

			vkCmdCopyBuffer(scope.GetCommandBuffer(), stagingBuffer.GetBuffer(), m_Buffer, 1, &copy);
		}

		VkBufferDeviceAddressInfo addressInfo{};
		addressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
		addressInfo.buffer = m_Buffer;

		m_DeviceAddress = vkGetBufferDeviceAddress(graphicsContext.GetDevice(), &addressInfo);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		vmaDestroyBuffer(graphicsContext.GetAllocator(), m_Buffer, m_Allocation);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VulkanIndexBuffer::VulkanIndexBuffer(void* data, uint32_t count) : m_IndexCount(count)
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		bufferInfo.size = sizeof(uint32_t) * count;

		VmaAllocationCreateInfo allocationInfo{};
		allocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		vmaCreateBuffer(graphicsContext.GetAllocator(), &bufferInfo, &allocationInfo, &m_Buffer, &m_Allocation, &m_AllocationInfo);

		VulkanStagingBuffer stagingBuffer(bufferInfo.size);

		memcpy(stagingBuffer.GetMappedMemory(), data, bufferInfo.size);

		{
			VkBufferCopy copy{};
			copy.size = bufferInfo.size;

			VulkanImmediateCommandScope scope;

			vkCmdCopyBuffer(scope.GetCommandBuffer(), stagingBuffer.GetBuffer(), m_Buffer, 1, &copy);
		}

	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		vmaDestroyBuffer(graphicsContext.GetAllocator(), m_Buffer, m_Allocation);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t size, const void* data, bool isDynamic) : m_Size(size), m_IsDynamic(isDynamic)
	{
		LY_CORE_ASSERT(!isDynamic | (isDynamic && size % 64 == 0), "Dynamic buffer sizes need to be aligned to 64");

		auto& graphicsContext = VulkanGraphicsContext::Get();
		auto& renderCommandBuffer = reinterpret_cast<VulkanRenderCommandBuffer&>(Renderer::GetRenderCommandBuffer());

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		bufferInfo.size = size;

		VmaAllocationCreateInfo allocationInfo{};

		if (isDynamic)
		{
			allocationInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
			allocationInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
			bufferInfo.size *= renderCommandBuffer.GetMaxFramesInFlight();
		}
		
		else
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			allocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		}

		vmaCreateBuffer(graphicsContext.GetAllocator(), &bufferInfo, &allocationInfo, &m_Buffer, &m_Allocation, &m_AllocationInfo);

		if (data)
		{
			VulkanStagingBuffer stagingBuffer(size);

			memcpy(stagingBuffer.GetMappedMemory(), data, size);

			{
				VkBufferCopy copy{};
				copy.size = size;

				VulkanImmediateCommandScope scope;

				vkCmdCopyBuffer(scope.GetCommandBuffer(), stagingBuffer.GetBuffer(), m_Buffer, 1, &copy);
			}
		}

	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		vmaDestroyBuffer(graphicsContext.GetAllocator(), m_Buffer, m_Allocation);
	}

	void VulkanUniformBuffer::UploadData(const void* data, uint32_t size, uint32_t offset)
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();
		auto& renderCommandBuffer = reinterpret_cast<VulkanRenderCommandBuffer&>(Renderer::GetRenderCommandBuffer());

		uint32_t bufferOffset = m_Size * renderCommandBuffer.GetCurrentFrameIndex() + offset;

		char* dst = static_cast<char*>(m_AllocationInfo.pMappedData) + bufferOffset;

		memcpy(dst, data, size);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VulkanStagingBuffer::VulkanStagingBuffer(uint32_t size)
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.size = size;

		VmaAllocationCreateInfo allocationInfo{};
		allocationInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
		allocationInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		vmaCreateBuffer(graphicsContext.GetAllocator(), &bufferInfo, &allocationInfo, &m_Buffer, &m_Allocation, &m_AllocationInfo);
	}

	VulkanStagingBuffer::~VulkanStagingBuffer()
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		vmaDestroyBuffer(graphicsContext.GetAllocator(), m_Buffer, m_Allocation);
	}
}
