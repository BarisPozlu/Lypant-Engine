#pragma once

#include <Lypant/Renderer/RenderPass.h>
#include <vulkan/vulkan.h>
#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "VulkanShader.h"
#include "VulkanPipeline.h"
#include "VulkanDescriptorSet.h"
#include "VulkanRenderTarget.h"

namespace lypant
{
	class VulkanSubpass : public Subpass
	{
	public:
		VulkanSubpass(const std::shared_ptr<RenderTarget>& renderTarget, const RenderTargetOperation& op, const std::shared_ptr<Shader>& shader, const std::vector<ImageBinding>& imageBindings, uint32_t uniformBufferSize, const void* data, bool isDynamic, int subpassFlags);
		virtual ~VulkanSubpass() = default;
		virtual void UploadData(const void* data, uint32_t size, uint32_t offset) override;
		inline virtual void SetRenderTarget(const std::shared_ptr<RenderTarget>& renderTarget) override { m_RenderTarget = reinterpret_cast<const std::shared_ptr<VulkanRenderTarget>&>(renderTarget); }
		inline virtual const std::shared_ptr<Shader>& GetShader() const override { return reinterpret_cast<const std::shared_ptr<Shader>&>(m_Shader); }
		inline virtual int GetFlags() const override { return m_SubpassFlags; };

		inline const std::shared_ptr<VulkanRenderTarget>& GetRenderTarget() const { return m_RenderTarget; }
		inline const std::shared_ptr<VulkanShader>& GetVkShader() const { return m_Shader; }
		inline const std::vector<ImageBinding>& GetDataBindings() const { return m_DataBindings; }
		inline const std::shared_ptr<VulkanGraphicsPipeline>& GetGraphicsPipeline() const { return m_GraphicsPipeline; }
		inline const std::shared_ptr<VulkanBuffer>& GetUniformBuffer() const { return m_UniformBuffer; }
		inline const std::shared_ptr<VulkanDescriptorSet>& GetDescriptorSet() const { return m_DescriptorSet; }
		inline const RenderTargetOperation& GetRenderTargetOperation() const { return m_RenderTargetOperation; }
	private:
		std::shared_ptr<VulkanRenderTarget> m_RenderTarget;
		std::shared_ptr<VulkanShader> m_Shader;
		std::vector<ImageBinding> m_DataBindings;
		std::shared_ptr<VulkanGraphicsPipeline> m_GraphicsPipeline;
		std::shared_ptr<VulkanBuffer> m_UniformBuffer;
		std::shared_ptr<VulkanDescriptorSet> m_DescriptorSet;
		int m_SubpassFlags;
		RenderTargetOperation m_RenderTargetOperation;
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