#pragma once

#include "Lypant/Renderer/GraphicsContext.h"
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

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
		static const std::unique_ptr<VulkanGraphicsContext>& Get();
		inline VkDevice GetDevice() const { return m_Device; }
		inline VkSurfaceKHR GetSurface() const { return m_Surface; }
		inline uint32_t GetGraphicsQueueFamilyIndex() const { return m_GraphicsQueueFamilyIndex; }
		inline VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
		inline const VulkanDeviceSurfaceDetails& GetDeviceSurfaceDetails() const { return m_DeviceSurfaceDetails; }
		inline VulkanSwapChain& GetSwapChain() { return *m_SwapChain; }
	private:
		void CreateInstance();
		void CreateSurface(GLFWwindow* windowHandle);
		void CreateDevice();
		bool IsDeviceSuitable(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceProperties& properties, const VkPhysicalDeviceFeatures& features);
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
		VulkanSwapChain* m_SwapChain;
	};
}
