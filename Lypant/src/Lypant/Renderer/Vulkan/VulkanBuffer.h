#pragma once

#include <Lypant/Renderer/Buffer.h>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace lypant
{
	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(void* data, uint32_t size);
		virtual ~VulkanVertexBuffer();
		inline VkDeviceAddress GetDeviceAddress() const { return m_DeviceAddress; }
	private:
		VkBuffer m_Buffer;
		VmaAllocation m_Allocation;
		VmaAllocationInfo m_AllocationInfo;
		VkDeviceAddress m_DeviceAddress;
	};

	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(void* data, uint32_t count);
		virtual ~VulkanIndexBuffer();
		inline VkBuffer Get() const { return m_Buffer; }
	private:
		VkBuffer m_Buffer;
		VmaAllocation m_Allocation;
		VmaAllocationInfo m_AllocationInfo;
	};

	class VulkanStagingBuffer
	{
	public:
		VulkanStagingBuffer(uint32_t size);
		~VulkanStagingBuffer();
		inline VkBuffer GetBuffer() const { return m_Buffer; }
		inline void* GetMappedMemory() const { return m_AllocationInfo.pMappedData; }
	private:
		VkBuffer m_Buffer;
		VmaAllocation m_Allocation;
		VmaAllocationInfo m_AllocationInfo;
	};
}
