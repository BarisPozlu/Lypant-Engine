#pragma once

#include <Lypant/Renderer/Buffer.h>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace lypant
{
	class VulkanBuffer : public Buffer
	{
	public:
		VulkanBuffer(const void* data, const BufferSpecification& spec);
		virtual ~VulkanBuffer();
		virtual void UploadData(const void* data, uint32_t size, uint32_t offset) override;
		inline VkBuffer GetVkBuffer() const { return m_Buffer; }
		inline uint32_t GetSize() const { return m_InputSize; }
		inline bool IsDynamic() const { return m_IsDynamic; }
		inline void* GetMappedMemory() const { LY_CORE_ASSERT(m_AllocationInfo.pMappedData, "Buffer was created as GPU local."); return m_AllocationInfo.pMappedData; }
		inline VkDeviceAddress GetDeviceAddress() const { LY_CORE_ASSERT(m_DeviceAddress, "Buffer was not created with a device address usage flag.") return m_DeviceAddress; }
		inline BufferType GetBufferType() const { return m_BufferType; }
	private:
		VkBuffer m_Buffer;
		VmaAllocation m_Allocation;
		VmaAllocationInfo m_AllocationInfo;
		VkDeviceAddress m_DeviceAddress;
		uint32_t m_InputSize; // NOTE: May not be actual size, since the allocation might be greater, or if dynamic, it is multiplied by frames in flight for the actual size.
		BufferType m_BufferType;
		bool m_IsDynamic;
	};
}
