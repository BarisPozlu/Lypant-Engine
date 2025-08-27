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
		inline uint32_t GetIndexCount() const { return m_IndexCount; }
		inline VkBuffer GetVkBuffer() const { return m_Buffer; }
	private:
		VkBuffer m_Buffer;
		VmaAllocation m_Allocation;
		VmaAllocationInfo m_AllocationInfo;
		uint32_t m_IndexCount;
	};

	// NOTE: The size is multiplied by the number of frames in flight. Uniform buffers are divided into sub-buffers that correspond to different frames in flight.
	class VulkanUniformBuffer : public UniformBuffer
	{
	public:
		VulkanUniformBuffer(uint32_t size, const void* data, bool isDynamic = false);
		virtual ~VulkanUniformBuffer();
		virtual void UploadData(const void* data, uint32_t size, uint32_t offset) override;
		inline VkBuffer GetVkBuffer() const { return m_Buffer; }
		inline uint32_t GetSize() const { return m_Size; }
		inline bool IsDynamic() const { return m_IsDynamic; }
	private:
		VkBuffer m_Buffer;
		VmaAllocation m_Allocation;
		VmaAllocationInfo m_AllocationInfo;
		uint32_t m_Size;
		bool m_IsDynamic;
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
