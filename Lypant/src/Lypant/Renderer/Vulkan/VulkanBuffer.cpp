#include <lypch.h>
#include "VulkanBuffer.h"
#include "VulkanGraphicsContext.h"
#include "VulkanCommandBuffer.h"

namespace lypant
{
	static VkBufferUsageFlags GetBufferUsageFlags(int bufferUsageFlags)
	{
		VkBufferUsageFlags flags = 0;

		if (bufferUsageFlags & BufferUsageFlagsTransferSrc)
		{
			flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		}

		if (bufferUsageFlags & BufferUsageFlagsTransferDst)
		{
			flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		}

		// NOTE: Engine does not use vulkan vertex buffers. Instead it uses storage buffers with device addresses for vertex data.
		if (bufferUsageFlags & BufferUsageFlagsVertexBuffer)
		{
			flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}

		// NOTE: Engine does not use vulkan index buffers. Instead it uses storage buffers with device addresses for index data.
		if (bufferUsageFlags & BufferUsageFlagsIndexBuffer)
		{
			flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}

		if (bufferUsageFlags & BufferUsageFlagsUniformBuffer)
		{
			flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		}

		if (bufferUsageFlags & BufferUsageFlagsStorageBuffer)
		{
			flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}

		if (bufferUsageFlags & BufferUsageFlagsIndirectBuffer)
		{
			flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
		}

		if (bufferUsageFlags & BufferUsageFlagsDeviceAddress)
		{
			flags |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
		}

		return flags;
	}

	VulkanBuffer::VulkanBuffer(const void* data, const BufferSpecification& spec)
	{
		m_BufferType = spec.Type;
		m_IsDynamic = spec.IsDynamic;
		m_InputSize = spec.Size;

		LY_CORE_ASSERT(!m_IsDynamic | (m_IsDynamic && m_InputSize % 64 == 0), "Dynamic buffer sizes need to be aligned to 64");

		auto& graphicsContext = VulkanGraphicsContext::Get();

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.usage = GetBufferUsageFlags(spec.UsageFlags);
		bufferInfo.size = m_IsDynamic ? m_InputSize * VulkanGraphicsContext::s_MaxFramesInFlight : m_InputSize;

		VmaAllocationCreateInfo allocationInfo{};
		allocationInfo.usage = (spec.MemoryFlags & BufferMemoryFlagsGPULocal) ?  VMA_MEMORY_USAGE_GPU_ONLY : VMA_MEMORY_USAGE_CPU_ONLY;
		allocationInfo.flags = (spec.MemoryFlags & BufferMemoryFlagsHostVisible) ? VMA_ALLOCATION_CREATE_MAPPED_BIT : 0;

		vmaCreateBuffer(graphicsContext.GetAllocator(), &bufferInfo, &allocationInfo, &m_Buffer, &m_Allocation, &m_AllocationInfo);

		if (data)
		{
			if (m_IsDynamic)
			{
				UploadData(data, m_InputSize, 0);
			}

			else
			{
				auto& stagingBuffer = reinterpret_cast<std::shared_ptr<VulkanBuffer>&>(Buffer::CreateStagingBuffer(m_InputSize));

				memcpy(stagingBuffer->GetMappedMemory(), data, m_InputSize);

				VulkanImmediateCommandBuffer commandBuffer;

				VkBufferCopy copy{};
				copy.size = m_InputSize;

				commandBuffer.BeginCommands();

				vkCmdCopyBuffer(commandBuffer.GetVkCommandBuffer(), stagingBuffer->GetVkBuffer(), m_Buffer, 1, &copy);

				commandBuffer.EndCommands();
			}
		}

		if (spec.UsageFlags & BufferUsageFlagsDeviceAddress)
		{
			VkBufferDeviceAddressInfo addressInfo{};
			addressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
			addressInfo.buffer = m_Buffer;

			m_DeviceAddress = vkGetBufferDeviceAddress(graphicsContext.GetDevice(), &addressInfo);
		}

		else
		{
			m_DeviceAddress = 0;
		}
	}

	VulkanBuffer::~VulkanBuffer()
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		VkBuffer buffer = m_Buffer;
		VmaAllocation allocation = m_Allocation;

		graphicsContext.GetDeletionQueue().PushFunction([buffer, allocation]()
			{
				auto& graphicsContext = VulkanGraphicsContext::Get();

				vmaDestroyBuffer(graphicsContext.GetAllocator(), buffer, allocation);
			});
	}

	void VulkanBuffer::UploadData(const void* data, uint32_t size, uint32_t offset)
	{
		LY_CORE_ASSERT(m_IsDynamic, "Static buffers do not support uploading data after creation.");

		auto& graphicsContext = VulkanGraphicsContext::Get();

		uint32_t bufferOffset = m_InputSize * graphicsContext.GetCurrentFrameIndex() + offset;

		char* dst = static_cast<char*>(m_AllocationInfo.pMappedData) + bufferOffset;

		memcpy(dst, data, size);
	}
}
