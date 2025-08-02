#pragma once

#include "Lypant/Renderer/GraphicsContext.h"
#include "Lypant/Core/Application.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

struct GLFWwindow;

namespace lypant
{
	class VulkanSwapChain;

	struct VulkanDeviceSurfaceDetails
	{
		VkSurfaceCapabilitiesKHR Capabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> Modes;
	};

	class VulkanGraphicsContext : public GraphicsContext
	{
	public:
		VulkanGraphicsContext(GLFWwindow* windowHandle);
		virtual ~VulkanGraphicsContext();
		inline static VulkanGraphicsContext& Get() { return reinterpret_cast<VulkanGraphicsContext&>(Application::Get().GetGraphicsContext()); }
		inline VkInstance GetInstance() const { return m_Instance; }
		inline VkSurfaceKHR GetSurface() const { return m_Surface; }
		inline VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
		inline VkDevice GetDevice() const { return m_Device; }
		inline const VulkanDeviceSurfaceDetails& GetDeviceSurfaceDetails() const { return m_DeviceSurfaceDetails; }
		inline uint32_t GetGraphicsQueueFamilyIndex() const { return m_GraphicsQueueFamilyIndex; }
		inline VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
		inline VulkanSwapChain& GetSwapChain() { return *m_SwapChain; }
		inline VmaAllocator GetAllocator() const { return m_Allocator; }
	private:
		void CreateInstance();
		void CreateSurface(GLFWwindow* windowHandle);
		void CreateDevice();
		bool IsDeviceSuitable(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceProperties2& properties, const VkPhysicalDeviceFeatures2& features, const VkPhysicalDeviceVulkan12Features& vk12Features);
	private:
		VkInstance m_Instance = VK_NULL_HANDLE;
		#ifdef LYPANT_DEBUG
		VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
		#endif 
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_Device = VK_NULL_HANDLE;
		VulkanDeviceSurfaceDetails m_DeviceSurfaceDetails;
		uint32_t m_GraphicsQueueFamilyIndex = UINT32_MAX;
		VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
		VulkanSwapChain* m_SwapChain = nullptr;
		VmaAllocator m_Allocator = nullptr;
	};
}
