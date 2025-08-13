#pragma once

#include <Lypant/Renderer/RenderPass.h>
#include <vulkan/vulkan.h>
#include "VulkanImage.h"
#include "VulkanShader.h"
#include "VulkanPipeline.h"
#include "VulkanDescriptorSet.h"
#include "VulkanRenderTarget.h"

// TODO: Draw commands are not recoreded in passes for now. The system will slightly change when that is introduced.

namespace lypant
{
	class VulkanSubpass : public Subpass
	{
	public:
		VulkanSubpass(const std::shared_ptr<RenderTarget>& renderTarget, const std::shared_ptr<Shader>& shader, const std::vector<DataBinding>& dataBindings);
		virtual ~VulkanSubpass() = default;
		inline const std::shared_ptr<VulkanRenderTarget>& GetRenderTarget() const { return m_RenderTarget; }
		inline const std::shared_ptr<VulkanShader>& GetShader() const { return m_Shader; }
		inline const std::shared_ptr<VulkanGraphicsPipeline>& GetGraphicsPipeline() const { return m_GraphicsPipeline; }
		inline const std::shared_ptr<VulkanDescriptorSet>& GetDescriptorSet() const { return m_DescriptorSet; }
	private:
		std::shared_ptr<VulkanRenderTarget> m_RenderTarget;
		std::shared_ptr<VulkanShader> m_Shader;
		std::shared_ptr<VulkanGraphicsPipeline> m_GraphicsPipeline;
		std::shared_ptr<VulkanDescriptorSet> m_DescriptorSet;
	};

	class VulkanRenderPass : public RenderPass
	{
	public:
		VulkanRenderPass();
		virtual ~VulkanRenderPass();
	private:
		std::vector<VulkanSubpass> m_Subpasses;
		int m_CurrentPass = -1;
	};
}