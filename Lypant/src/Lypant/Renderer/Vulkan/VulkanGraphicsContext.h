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
		// NOTE: Vulkan graphics context is needed in most vulkan resources. This acts as a convenience function. Might be bad.
		inline static VulkanGraphicsContext& Get()
		{	
			LY_CORE_ASSERT(GraphicsContext::GetGraphicsAPI() == GraphicsAPI::Vulkan, "Trying to get vulkan graphics context while selected api is not vulkan");
			return reinterpret_cast<VulkanGraphicsContext&>(Application::Get().GetGraphicsContext());
		}
		inline VkInstance GetInstance() const { return m_Instance; }
		inline VkSurfaceKHR GetSurface() const { return m_Surface; }
		inline VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
		inline VkDevice GetDevice() const { return m_Device; }
		inline const VulkanDeviceSurfaceDetails& GetDeviceSurfaceDetails() const { return m_DeviceSurfaceDetails; }
		inline uint32_t GetGraphicsQueueFamilyIndex() const { return m_GraphicsQueueFamilyIndex; }
		inline VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
		inline VulkanSwapChain& GetSwapChain() { return *m_SwapChain; }
		inline VmaAllocator GetAllocator() const { return m_VmaAllocator; }
		inline float GetMaxSamplerAnisotropy() const { return m_MaxSamplerAnisotropy; }
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
		VmaAllocator m_VmaAllocator = nullptr;
		float m_MaxSamplerAnisotropy = 1;
	};
}
