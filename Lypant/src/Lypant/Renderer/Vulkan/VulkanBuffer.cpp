#include <lypch.h>
#include "VulkanBuffer.h"
#include "VulkanGraphicsContext.h"
#include "VulkanCommandBuffer.h"

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
