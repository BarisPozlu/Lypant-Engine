#pragma once

#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace lypant
{
	struct DeviceSurfaceDetails
	{
		VkSurfaceCapabilitiesKHR Capabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> Modes;
	};

	class GraphicsContext
	{
	public:
		GraphicsContext(GLFWwindow* windowHandle);
		~GraphicsContext();
		inline VkDevice GetDevice() const { return m_Device; }
		inline VkSurfaceKHR GetSurface() const { return m_Surface; }
		inline const DeviceSurfaceDetails& GetDeviceSurfaceDetails() const { return m_DeviceSurfaceDetails; }
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
		DeviceSurfaceDetails m_DeviceSurfaceDetails;
		uint32_t m_GraphicsQueueIndex = UINT32_MAX;
		std::unique_ptr<class SwapChain> m_SwapChain;
	};
}
