#pragma once

#include "Lypant/ImGui/ImGuiLayer.h"

namespace lypant
{
	class VulkanImGuiLayer : public ImGuiLayer
	{
	public:
		VulkanImGuiLayer();
		virtual ~VulkanImGuiLayer();
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void Begin() override;
		virtual void End() override;
	};
}
