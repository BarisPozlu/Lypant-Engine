#include "lypch.h"
#include "VulkanGraphicsContext.h"
#include <GLFW/glfw3.h>
#include "VulkanSwapChain.h"

namespace lypant
{
	static constexpr std::array<const char*, 1> s_RequiredLayers = { "VK_LAYER_KHRONOS_validation" };
	//TODO: dynamic rendering is not extension in vk 1.3, enable it in the 1.3 features
	static constexpr std::array<const char*, 2> s_RequiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME };

	VulkanGraphicsContext::VulkanGraphicsContext(GLFWwindow* windowHandle)
	{
		CreateInstance();
		CreateSurface(windowHandle);
		CreateDevice();
		vkGetDeviceQueue(m_Device, m_GraphicsQueueFamilyIndex, 0, &m_GraphicsQueue);
		m_SwapChain = new VulkanSwapChain(m_Device, m_Surface, m_DeviceSurfaceDetails);

		VmaAllocatorCreateInfo allocatorInfo{};
		allocatorInfo.instance = m_Instance;
		allocatorInfo.physicalDevice = m_PhysicalDevice;
		allocatorInfo.device = m_Device;
		allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
		vmaCreateAllocator(&allocatorInfo, &m_Allocator);
	}

	VulkanGraphicsContext::~VulkanGraphicsContext()
	{
		vmaDestroyAllocator(m_Allocator);
		delete m_SwapChain;
		vkDestroyDevice(m_Device, nullptr);
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		auto DestroyDebugMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");
		DestroyDebugMessenger(m_Instance, m_DebugMessenger, nullptr);
		vkDestroyInstance(m_Instance, nullptr);
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		{
			LY_CORE_ERROR("Validation layer: {0}", pCallbackData->pMessage);
			LY_CORE_ASSERT(false, "Vulkan debug callback");
		}

		else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		{
			LY_CORE_WARNING("Validation layer: {0}", pCallbackData->pMessage);
		}

		return VK_FALSE;
	}

	static void EnableValidationLayers(VkInstanceCreateInfo* instanceInfo)
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> layers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

		for (const char* requiredLayer : s_RequiredLayers)
		{
			bool found = false;

			for (const auto& layer : layers)
			{
				if (std::strcmp(layer.layerName, requiredLayer) == 0)
				{
					found = true;
					break;
				}
			}

			LY_CORE_ASSERT(found, "Could not find requested layers");
		}

		instanceInfo->enabledLayerCount = s_RequiredLayers.size();
		instanceInfo->ppEnabledLayerNames = s_RequiredLayers.data();
	}

	void VulkanGraphicsContext::CreateInstance()
	{
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Sandbox";
		appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
		appInfo.pEngineName = "Lypant Engine";
		appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_4;

		VkInstanceCreateInfo instanceInfo{};
		instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceInfo.pApplicationInfo = &appInfo;

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		#ifdef LYPANT_DEBUG
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		#endif

		instanceInfo.enabledExtensionCount = extensions.size();
		instanceInfo.ppEnabledExtensionNames = extensions.data();

		instanceInfo.enabledLayerCount = 0;

		#ifdef LYPANT_DEBUG
		VkDebugUtilsMessengerCreateInfoEXT messengerInfo{};
		messengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		messengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		messengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		messengerInfo.pfnUserCallback = DebugCallback;

		instanceInfo.pNext = &messengerInfo;
		EnableValidationLayers(&instanceInfo);
		#endif

		vkCreateInstance(&instanceInfo, nullptr, &m_Instance);

		// TODO: these functions should load once during start up, move them somewhere else
		#ifdef LYPANT_DEBUG
		auto CreateDebugMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
		CreateDebugMessenger(m_Instance, &messengerInfo, nullptr, &m_DebugMessenger);
		#endif
	}

	void VulkanGraphicsContext::CreateSurface(GLFWwindow* windowHandle)
	{
		glfwCreateWindowSurface(m_Instance, windowHandle, nullptr, &m_Surface);
	}

	void VulkanGraphicsContext::CreateDevice()
	{
		uint32_t deviceCount;
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

		std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, physicalDevices.data());

		VkPhysicalDeviceProperties2 properties{};
		properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		VkPhysicalDeviceVulkan12Features vk12features{};
		vk12features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		VkPhysicalDeviceFeatures2 features{};
		features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		features.pNext = &vk12features;

		for (auto physicalDevice : physicalDevices)
		{
			vkGetPhysicalDeviceProperties2(physicalDevice, &properties);
			vkGetPhysicalDeviceFeatures2(physicalDevice, &features);

			if (IsDeviceSuitable(physicalDevice, properties, features, vk12features))
			{
				m_PhysicalDevice = physicalDevice;
				if (properties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) break;
			}
		}

		LY_CORE_ASSERT(m_PhysicalDevice != VK_NULL_HANDLE, "Could not select a physical device");

		VkDeviceQueueCreateInfo queueInfo{};
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.queueCount = 1;
		queueInfo.queueFamilyIndex = m_GraphicsQueueFamilyIndex;
		float priority = 1;
		queueInfo.pQueuePriorities = &priority;

		VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeatures{};
		dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
		dynamicRenderingFeatures.dynamicRendering = VK_TRUE;

		VkPhysicalDeviceVulkan12Features enabledvk12Features{};
		enabledvk12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		enabledvk12Features.bufferDeviceAddress = VK_TRUE;
		enabledvk12Features.descriptorIndexing = VK_TRUE;
		enabledvk12Features.pNext = &dynamicRenderingFeatures;

		VkPhysicalDeviceFeatures2 enabledFeatures{};
		enabledFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		enabledFeatures.features.geometryShader = VK_TRUE;
		enabledFeatures.features.samplerAnisotropy = VK_TRUE;
		enabledFeatures.pNext = &enabledvk12Features;

		VkDeviceCreateInfo deviceInfo{};
		deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceInfo.pNext = &enabledFeatures;
		deviceInfo.queueCreateInfoCount = 1;
		deviceInfo.pQueueCreateInfos = &queueInfo;
		deviceInfo.enabledExtensionCount = s_RequiredDeviceExtensions.size();
		deviceInfo.ppEnabledExtensionNames = s_RequiredDeviceExtensions.data();

		vkCreateDevice(m_PhysicalDevice, &deviceInfo, nullptr, &m_Device);

		LY_CORE_ASSERT(m_Device != VK_NULL_HANDLE, "Could not select a device");
		LY_CORE_INFO("Selected device: {0}", properties.properties.deviceName);
	}

	bool VulkanGraphicsContext::IsDeviceSuitable(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceProperties2& properties, const VkPhysicalDeviceFeatures2& features, const VkPhysicalDeviceVulkan12Features& vk12Features)
	{
		// Geometry shader and anisotropy filtering check
		if (!features.features.geometryShader || !features.features.samplerAnisotropy) return false;

		// Buffer device address and descriptor indexing check
		if (!vk12Features.bufferDeviceAddress || !vk12Features.descriptorIndexing) return false;

		// Extension checks
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensions.data());

		for (const char* requiredExtension : s_RequiredDeviceExtensions)
		{
			bool found = false;

			for (const auto& extension : extensions)
			{
				if (std::strcmp(requiredExtension, extension.extensionName) == 0)
				{
					found = true;
					break;
				}
			}

			if (!found) return false;
		}

		// Swap chain check
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_Surface, &m_DeviceSurfaceDetails.Capabilities);

		uint32_t formatCount;

		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, nullptr);
		if (formatCount != 0)
		{
			m_DeviceSurfaceDetails.Formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, m_DeviceSurfaceDetails.Formats.data());
		}

		uint32_t modeCount;

		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &modeCount, nullptr);
		if (modeCount != 0)
		{
			m_DeviceSurfaceDetails.Modes.resize(modeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &modeCount, m_DeviceSurfaceDetails.Modes.data());
		}

		if (m_DeviceSurfaceDetails.Formats.empty() || m_DeviceSurfaceDetails.Modes.empty()) return false;

		// Queue check
		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

		for (int i = 0; i < queueFamilies.size(); i++)
		{
			const auto& queueFamily = queueFamilies[i];

			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				VkBool32 supported = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, m_Surface, &supported);
				if (supported)
				{
					m_GraphicsQueueFamilyIndex = i;
					return true;
				}
			}
		}

		return false;
	}

}