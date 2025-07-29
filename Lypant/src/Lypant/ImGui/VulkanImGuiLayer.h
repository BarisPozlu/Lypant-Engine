#pragma once

#include "Lypant/ImGui/ImGuiLayer.h"
#include <vulkan/vulkan.h>

namespace lypant
{
	class VulkanImGuiLayer : public ImGuiLayer
	{
	public:
		VulkanImGuiLayer();
		virtual ~VulkanImGuiLayer();
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		void Begin();
		void End();
	};
}
