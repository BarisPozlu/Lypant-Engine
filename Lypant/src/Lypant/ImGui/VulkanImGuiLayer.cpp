#include "lypch.h"
#include "VulkanImGuiLayer.h"
#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"
#include "Lypant/Core/Application.h"
#include <Lypant/Renderer/Vulkan/VulkanRenderCommandBuffer.h>
#include <Lypant/Renderer/Vulkan/VulkanGraphicsContext.h>
#include <Lypant/Renderer/Vulkan/VulkanSwapChain.h>
#include <GLFW/glfw3.h>


#include <Lypant/Renderer/Renderer.h>

namespace lypant
{
	VulkanImGuiLayer::VulkanImGuiLayer()
	{

	}

	VulkanImGuiLayer::~VulkanImGuiLayer()
	{

	}

	void VulkanImGuiLayer::OnAttach()
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImGui_ImplGlfw_InitForVulkan(Application::Get().GetWindow().GetGLFWwindow(), true);

		ImGui_ImplVulkan_InitInfo init_info{};
		init_info.Instance = graphicsContext.GetInstance();
		init_info.PhysicalDevice = graphicsContext.GetPhysicalDevice();
		init_info.Device = graphicsContext.GetDevice();
		init_info.Queue = graphicsContext.GetGraphicsQueue();
		init_info.DescriptorPoolSize = IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE;
		init_info.MinImageCount = 3;
		init_info.ImageCount = 3;
		init_info.UseDynamicRendering = true;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		
		init_info.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
		VkFormat format = graphicsContext.GetSwapChain().GetImageFormat();
		init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &format;

		ImGui_ImplVulkan_Init(&init_info);
	}

	void VulkanImGuiLayer::OnDetach()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void VulkanImGuiLayer::Begin()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void VulkanImGuiLayer::End()
	{
		ImGui::Render();
		VkCommandBuffer commandBuffer = reinterpret_cast<VulkanRenderCommandBuffer&>(Renderer::GetRenderCommandBuffer()).GetCommandBuffer();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

}
