#pragma once

#include <Lypant/Renderer/RenderPass.h>
#include <vulkan/vulkan.h>
#include "VulkanBuffer.h"
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
		VulkanSubpass(const std::shared_ptr<RenderTarget>& renderTarget, bool shouldClear, const std::shared_ptr<Shader>& shader, const std::vector<DataBinding>& dataBindings, uint32_t uniformBufferSize, const void* data, bool isDynamic = false);
		virtual ~VulkanSubpass() = default;
		virtual void UploadData(const void* data, uint32_t size, uint32_t offset) override;
		inline const std::shared_ptr<VulkanRenderTarget>& GetRenderTarget() const { return m_RenderTarget; }
		inline const std::shared_ptr<VulkanShader>& GetShader() const { return m_Shader; }
		inline const std::vector<DataBinding>& GetDataBindings() const { return m_DataBindings; }
		inline const std::shared_ptr<VulkanGraphicsPipeline>& GetGraphicsPipeline() const { return m_GraphicsPipeline; }
		inline const std::shared_ptr<VulkanBuffer>& GetUniformBuffer() const { return m_UniformBuffer; }
		inline const std::shared_ptr<VulkanDescriptorSet>& GetDescriptorSet() const { return m_DescriptorSet; }
		inline bool ShouldClearTarget() const { return m_ShouldClearTarget; }
	private:
		std::shared_ptr<VulkanRenderTarget> m_RenderTarget;
		std::shared_ptr<VulkanShader> m_Shader;
		std::vector<DataBinding> m_DataBindings;
		std::shared_ptr<VulkanGraphicsPipeline> m_GraphicsPipeline;
		std::shared_ptr<VulkanBuffer> m_UniformBuffer;
		std::shared_ptr<VulkanDescriptorSet> m_DescriptorSet;
		bool m_ShouldClearTarget;
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